/*
* passthru.c
* (C) 2013, all rights reserved,
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
* DESCRIPTION:
* This program does nothing except divert packets and re-inject them.  This is
* useful for performance testing.
*
* usage: netdump.exe windivert-filter num-threads
*/

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib, "WSock32.lib")
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "windivert.h"

#include "PKURemoteUtil.h"
#include "CredData.h"

#define MAXBUF								0xFFFF

#define VMWARE_CONTROL_PORT					443
#define REDIRECTED_CONTROL_PORT_BASE		1005

#define VMWARE_MKS_PORT						902
#define REDIRECTED_MKS_PORT_BASE			1015

#define NETBIOS_PORT						139
#define REDIRECTED_NETBIOS_PORT				1029

#define CIFS_PORT							445
#define REDIRECTED_CIFS_PORT				1025

int g_iServerNo = 0;
int g_stopFlag = 0;

static DWORD passthru(LPVOID arg);

int g_thread_cnt = 0;
HANDLE g_threads[16] = {0};

/*
* Entry.
*/
int startWindivert(int iServerNo, int num_threads)
{
	int i;
	HANDLE handle, thread;
	g_iServerNo = iServerNo;
	g_stopFlag = 0;
	
	if (num_threads < 1 || num_threads > 64)
	{
		fprintf(stderr, "error: invalid number of threads\n");
		//exit(EXIT_FAILURE);
		return -1;
	}

	// Divert traffic matching the filter:
	handle = WinDivertOpen("true", WINDIVERT_LAYER_NETWORK, 0, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_INVALID_PARAMETER)
		{
			fprintf(stderr, "error: filter syntax error\n");
			//exit(EXIT_FAILURE);
			return -1;
		}
		fprintf(stderr, "error: failed to open the WinDivert device (%d)\n",
			GetLastError());
		//exit(EXIT_FAILURE);
		return -1;
	}

	// Start the threads
	g_thread_cnt = 0;
	for (i = 0; i < num_threads; i++)
	{
		thread = CreateThread(NULL, 1, (LPTHREAD_START_ROUTINE)passthru,
			(LPVOID)handle, 0, NULL);
		if (thread == NULL)
		{
			fprintf(stderr, "error: failed to start passthru thread (%u)\n",
				GetLastError());
			//exit(EXIT_FAILURE);
			return -1;
		}
		g_threads[i] = thread;
		g_thread_cnt ++;
	}

	// Main thread:
	// passthru((LPVOID)handle);

	return 0;
}

int stopWindivert()
{
	g_stopFlag = 1;
	for (int i = 0; i < g_thread_cnt; i ++)
	{
		WaitForSingleObject(g_threads[i], 0xffffffff);
		CloseHandle(g_threads[i]);
		g_threads[i] = NULL;
	}
	g_thread_cnt = 0;
	return 0;
}

USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
	{
		cksum += *(UCHAR *)buffer;
	}
	// Transfer 32 bits to 16 bits
	while (cksum >> 16)
		cksum = (cksum >> 16) + (cksum & 0xffff);
	return (USHORT)(~cksum);
}

USHORT checksum_init(USHORT *buffer, int size, USHORT init)
{
	unsigned long cksum = init;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
	{
		cksum += *(UCHAR *)buffer;
	}
	// Transfer 32 bits to 16 bits
	while (cksum >> 16)
		cksum = (cksum >> 16) + (cksum & 0xffff);
	return (USHORT)(~cksum);
}

// Passthru thread.
static DWORD passthru(LPVOID arg)
{
	unsigned char packet[MAXBUF];
	UINT packet_len;
	WINDIVERT_ADDRESS addr;
	HANDLE handle = (HANDLE)arg;
	PIP_HEADER pPacket = (PIP_HEADER) packet;
	PTCP_HEADER pTCPHeader;
	PUDP_HEADER pUDPHeader;
	int iServerIP = htonl(SERVER_IP);
	UINT8 *server_addr = (UINT8 *)&iServerIP;
	UINT8 *dst_addr;
	UINT8 *src_addr;
	TCP_Pseudo TCPPseudo;
	TCP_Pseudo *pTCPPseudo = &TCPPseudo;
	USHORT iCheckSum;
	int IPHeaderLen;
	int IPPayloadLen;

	// Main loop:
	while (TRUE)
	{
		if (g_stopFlag == 1)
			break;
		// Read a matching packet.
		if (!WinDivertRecv(handle, packet, sizeof(packet), &addr, &packet_len))
		{
			fprintf(stderr, "warning: failed to read packet (%d)\n",
				GetLastError());
			continue;
		}

		IPHeaderLen = (0x0F & pPacket->ip_hVerLen) * 4;
		IPPayloadLen = ntohs(pPacket->ip_Length) - IPHeaderLen;
		pTCPHeader = (PTCP_HEADER)(packet + IPHeaderLen);
		pUDPHeader = (PUDP_HEADER)pTCPHeader;
		dst_addr = (UINT8 *)&pPacket->ip_Dst;
		src_addr = (UINT8 *)&pPacket->ip_Src;
		
		//printf("version = %d, ip header size = %d\n", (0xF0 & (pPacket->ip_hVerLen)) >> 4, IPHeaderLen);

		// IPv6
		if ((0x0F & pPacket->ip_hVerLen) == 0)
		{
			goto JumpOver;
		}

// 		printf("ip_Dst = %u.%u.%u.%u, ip_Src = %u.%u.%u.%u, SERVER_IP = %u.%u.%u.%u\n",
// 			dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3],
// 			src_addr[0], src_addr[1], src_addr[2], src_addr[3],
// 			server_addr[0], server_addr[1], server_addr[2], server_addr[3]
// 			);

// 			printf("ip_Checksum = %x\n", ntohs(pPacket->ip_Checksum));
// 			pPacket->ip_Checksum = 0x0;
// 			pPacket->ip_Checksum = checksum((USHORT *)pPacket, 20);
// 			printf("New ip_Checksum = %x\n", ntohs(pPacket->ip_Checksum));

		if (pPacket->ip_Dst == htonl(SERVER_IP))
		{
			printf("Server IP got!! ip_Dst = %u.%u.%u.%u, ip_Protocol = %d, m_sDestPort = %d\n",
				dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3],
				pPacket->ip_Protocol, ntohs(pTCPHeader->m_sDestPort));
			if (pPacket->ip_Protocol == TCP_PORTO || pPacket->ip_Protocol == UDP_PORTO)
			{
				if (ntohs(pTCPHeader->m_sDestPort) == VMWARE_CONTROL_PORT)
				{
					pTCPHeader->m_sDestPort = htons(REDIRECTED_CONTROL_PORT_BASE + g_iServerNo);
				}
				else if (ntohs(pTCPHeader->m_sDestPort) == VMWARE_MKS_PORT)
				{
					pTCPHeader->m_sDestPort = htons(REDIRECTED_MKS_PORT_BASE + g_iServerNo);
				}
				else if (ntohs(pTCPHeader->m_sDestPort) == NETBIOS_PORT)
				{
					pTCPHeader->m_sDestPort = htons(REDIRECTED_NETBIOS_PORT);
				}
				else if (ntohs(pTCPHeader->m_sDestPort) == CIFS_PORT)
				{
					pTCPHeader->m_sDestPort = htons(REDIRECTED_CIFS_PORT);
				}
				else
				{
					goto JumpOver;
				}

				printf("New m_sDestPort = %d\n", ntohs(pTCPHeader->m_sDestPort));		
			}
		}
		else if (pPacket->ip_Src == htonl(SERVER_IP))
		{
			printf("Server IP got!! ip_Src = %u.%u.%u.%u, ip_Protocol = %d, m_sSourPort = %d\n",
				src_addr[0], src_addr[1], src_addr[2], src_addr[3],
				pPacket->ip_Protocol, ntohs(pTCPHeader->m_sSourPort));
			if (pPacket->ip_Protocol == TCP_PORTO || pPacket->ip_Protocol == UDP_PORTO)
			{
				if (ntohs(pTCPHeader->m_sSourPort) == REDIRECTED_CONTROL_PORT_BASE + g_iServerNo)
				{
					pTCPHeader->m_sSourPort = htons(VMWARE_CONTROL_PORT);
				}
				else if (ntohs(pTCPHeader->m_sSourPort) == REDIRECTED_MKS_PORT_BASE + g_iServerNo)
				{
					pTCPHeader->m_sSourPort = htons(VMWARE_MKS_PORT);
				}
				else if (ntohs(pTCPHeader->m_sSourPort) == REDIRECTED_NETBIOS_PORT)
				{
					pTCPHeader->m_sSourPort = htons(NETBIOS_PORT);
				}
				else if (ntohs(pTCPHeader->m_sSourPort) == REDIRECTED_CIFS_PORT)
				{
					pTCPHeader->m_sSourPort = htons(CIFS_PORT);
				}
				else
				{
					goto JumpOver;
				}

				printf("New m_sSourPort = %d\n", ntohs(pTCPHeader->m_sSourPort));
			}
		}
		else
		{
			goto JumpOver;
		}

		pPacket->ip_Checksum = 0x0;
		pPacket->ip_Checksum = checksum((USHORT *)pPacket, IPHeaderLen);
		if (pPacket->ip_Protocol == TCP_PORTO)
		{
			pTCPHeader->m_sCheckSum = 0x0;
			pTCPPseudo->src_ip = pPacket->ip_Src;
			pTCPPseudo->dest_ip = pPacket->ip_Dst;
			pTCPPseudo->zeroes = 0;
			pTCPPseudo->protocol = 6;
			pTCPPseudo->len = htons(IPPayloadLen);

			iCheckSum = checksum_init((USHORT*) pTCPPseudo, sizeof(TCPPseudo), 0);

			// Update the checksum by checksumming the TCP header
			// and data as if those had directly followed the pseudo header
			iCheckSum = checksum_init((USHORT*) pTCPHeader, IPPayloadLen, (USHORT)~iCheckSum);
			pTCPHeader->m_sCheckSum = iCheckSum;
		}
		else if (pPacket->ip_Protocol == UDP_PORTO)
		{

		}
		

		//printf("%d\n", packet_len);

JumpOver:
		// Re-inject the matching packet.
		if (!WinDivertSend(handle, packet, packet_len, &addr, NULL))
		{
			fprintf(stderr, "warning: failed to reinject packet (%d)\n",
				GetLastError());
		}
	}

	WinDivertClose(handle);

	return 0;
}
