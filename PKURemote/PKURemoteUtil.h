#pragma once

#pragma pack(push)
#pragma pack (1)

typedef struct _IP_HEADER
{
	UCHAR     ip_hVerLen;			/* Version (4 bits) + Internet header length (4 bits) */
	UCHAR     ip_TOS;				/* TOS Type of service */
	USHORT    ip_Length;			/* Total length */
	USHORT    ip_ID;				/* Identification */
	USHORT    ip_Flags;				/* Flags (3 bits) + Fragment offset (13 bits) */
	UCHAR     ip_TTL;				/* Time to live */
	UCHAR     ip_Protocol;			/* Protocol */
	USHORT    ip_Checksum;			/* Header checksum */
	ULONG     ip_Src;				/* Source address */
	ULONG     ip_Dst;				/* Destination address */
} IP_HEADER, *PIP_HEADER;

#define TCP_PORTO	6
#define UDP_PORTO	17

typedef struct _TCP_HEADER
{
	short m_sSourPort;					// 源端口号16bit
	short m_sDestPort;					// 目的端口号16bit
	unsigned int m_uiSequNum;			// 序列号32bit
	unsigned int m_uiAcknowledgeNum;	// 确认号32bit
	short m_sHeaderLenAndFlag;			// 前4位：TCP头长度；中6位：保留；后6位：标志位
	short m_sWindowSize;				// 窗口大小16bit
	short m_sCheckSum;					// 检验和16bit
	short m_surgentPointer;				// 紧急数据偏移量16bit
} TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER
{
	unsigned short m_usSourPort;		// 源端口号16bit
	unsigned short m_usDestPort;		// 目的端口号16bit
	unsigned short m_usLength;			// 数据包长度16bit
	unsigned short m_usCheckSum;		// 校验和16bit
} UDP_HEADER, *PUDP_HEADER;

typedef struct _TCP_Pseudo {
	ULONG src_ip; /* source ip */
	ULONG dest_ip; /* destination ip */
	UCHAR zeroes; /* = 0 */
	UCHAR protocol; /* = 6 */
	USHORT len; /* length of TCPHeader */
} TCP_Pseudo, *PTCP_Pseudo;

#pragma pack(pop)

int startWindivert(int iServerNo, int num_threads);
int stopWindivert();