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
	short m_sSourPort;					// Դ�˿ں�16bit
	short m_sDestPort;					// Ŀ�Ķ˿ں�16bit
	unsigned int m_uiSequNum;			// ���к�32bit
	unsigned int m_uiAcknowledgeNum;	// ȷ�Ϻ�32bit
	short m_sHeaderLenAndFlag;			// ǰ4λ��TCPͷ���ȣ���6λ����������6λ����־λ
	short m_sWindowSize;				// ���ڴ�С16bit
	short m_sCheckSum;					// �����16bit
	short m_surgentPointer;				// ��������ƫ����16bit
} TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER
{
	unsigned short m_usSourPort;		// Դ�˿ں�16bit
	unsigned short m_usDestPort;		// Ŀ�Ķ˿ں�16bit
	unsigned short m_usLength;			// ���ݰ�����16bit
	unsigned short m_usCheckSum;		// У���16bit
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