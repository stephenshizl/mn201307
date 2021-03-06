#include <linux/string.h>
#include <packet.h>
#include <usb/dl_stdio.h>
//#include "fdl_main.h"
#include <dl_crc.h>
#include <usb/usb_boot.h>
#include <usb/virtual_com.h>
#include <usb/dl_channel.h>

#define	MODIFY_REC_DATA	1

extern void dl_send_ack (dl_cmd_type_t pkt_type);


struct FDL_ChannelHandler *gFdlUsedChannel;


static dl_packet_t  packet[ PACKET_MAX_NUM ];

static dl_packet_t *packet_free_list;
static dl_packet_t *packet_completed_list;
static dl_packet_t *packet_receiving;

extern int act_as_romcode;

void dl_packet_init (void)
{
    uint32_t i = 0;

    packet_free_list = &packet[0];

    for (i = 0; i < PACKET_MAX_NUM; i++)
    {
        memset (&packet[i], 0, sizeof (dl_packet_t));
        packet[i].next   = &packet[i+1];
    }

    packet[PACKET_MAX_NUM-1].next = NULL;

    packet_completed_list = NULL;
    packet_receiving      = NULL;

    gFdlUsedChannel = FDL_ChannelGet();
}


dl_packet_t *FDL_MallocPacket (void)
{
    dl_packet_t   *tmp_ptr = NULL;

    if (NULL != packet_free_list)
    {
        tmp_ptr = packet_free_list;
        packet_free_list = tmp_ptr->next;

        // only clear the packet header
        memset (tmp_ptr, 0, sizeof(*tmp_ptr));

        tmp_ptr->next       = NULL;
        tmp_ptr->pkt_state  = PKT_NONE;
        tmp_ptr->ack_flag   = 0;
        tmp_ptr->data_size  = 0;
    }

    return tmp_ptr;
}

void dl_free_packet (dl_packet_t *ptr)
{
    ptr->next        = packet_free_list;
    packet_free_list = ptr;
}

dl_packet_t   *dl_get_packet (void)
{
    dl_packet_t *ptr;
//printf("bfgetpk,time=%u  \n", SCI_GetTickCount());

    // waiting for a packet
    while (NULL == packet_completed_list)
    {
        FDL_PacketDoIdle();
    }
//printf("aftgetpk,time=%u \n ", SCI_GetTickCount());

    // remove from completed list
    ptr                     = packet_completed_list;
    packet_completed_list   = ptr->next;
    ptr->next               = NULL;
    return ptr;
}

void FDL_PacketDoIdle (void)
{
    //FDL_BOOT_MODE_E          boot_mode;
    unsigned char *pdata      = NULL;
    dl_packet_t       *packet_ptr = NULL;
    dl_packet_t       *tmp_ptr    = NULL;
    uint32_t          crc;
    unsigned char   ch;
    int ch1;
	int nonhdlc_size;
	unsigned char *array;
	int aaa;

    // try get a packet to handle the receive char
    packet_ptr = packet_receiving;

    if (NULL == packet_ptr)
    {
        packet_ptr = FDL_MallocPacket();

        if (NULL != packet_ptr)
        {
            packet_receiving    = packet_ptr;
            packet_ptr->next    = NULL;
        }
        else
        {
            return ;
        }
    }

    pdata  = (unsigned char *) & (packet_ptr->body);

    while (1)
    {
        /*
        * here the reason that we don't call GetChar is
        * that GetChar() isnot exited until got data from
        * outside,FDL_PacketDoIdle() is called also in norflash write and erase
        * so we have to exited when no char recieived here,
        * usb virtual com is the same handle
        */
        ch1 = gFdlUsedChannel->GetSingleChar (gFdlUsedChannel);

        if (ch1 == -1)
        {
            return;
        }

        ch = ch1&0xff;

        if (packet_ptr->data_size > MAX_PKT_SIZE)
        {
            packet_receiving = NULL;
            dl_free_packet (packet_ptr);
            sio_trace ("data_size error : datasize = %d  MAX_PKT_SIZE = %d\n", packet_ptr->data_size, MAX_PKT_SIZE);
		printf("\n\n\n%s %s %d\n\n\n", __FILE__, __FUNCTION__, __LINE__);
		printf("data_size error");
		printf("\n\n\n%s %s %d\n\n\n", __FILE__, __FUNCTION__, __LINE__);
            SEND_ERROR_RSP (BSL_REP_VERIFY_ERROR)
            //return;
        }

        // try handle this input data
	//printf(" %02x  %d\n", ch1, packet_ptr->pkt_state);
        switch (packet_ptr->pkt_state)
        {
            case PKT_NONE:

                if (HDLC_FLAG == ch)
                {
                    packet_ptr->pkt_state = PKT_HEAD;
                    packet_ptr->data_size = 0;
                }

                break;
            case PKT_HEAD:

                if (HDLC_FLAG != ch)
                {
                    if (HDLC_ESCAPE == ch)
                    {
                        // Try get the "true" data.
                        //ch = sio_get_char();
                        ch = gFdlUsedChannel->GetChar (gFdlUsedChannel);
                        ch = ch ^ HDLC_ESCAPE_MASK;
                    }

                    packet_ptr->pkt_state = PKT_GATHER;
                    * (pdata + packet_ptr->data_size)         = ch;
                    //*(pdata++) = ch;
                    packet_ptr->data_size += 1;

                }

                break;
            case PKT_GATHER:
#if defined(MODIFY_REC_DATA)
		/////////////////////////////////////////////////////////////
                if (HDLC_FLAG == ch) {
			packet_ptr->pkt_state = PKT_RECV;
                    	//check the packet. CRC should be 0

			if (1 == act_as_romcode)
				crc = crc_16_l_calc((unsigned short *)&packet_ptr->body, packet_ptr->data_size);
			else
				crc = frm_chk ( (unsigned short *) &packet_ptr->body, packet_ptr->data_size);

                    	if (0 != crc) {
				int k;
				printf("%4x", packet_ptr->body.type);
				printf("%4x", packet_ptr->body.size);
				for(k = 0;  k  < packet_ptr->data_size - 4; k++)
				{
					printf("%2x", packet_ptr->body.content[k]);
				}
				printf("\n\n\n\n");
				for(k = 0;  k  < packet_ptr->data_size - 4; k++)
				{
					printf("%c", packet_ptr->body.content[k]);
				}
                        	//Verify error, reject this packet.
                        	dl_free_packet(packet_ptr);
                        	packet_receiving = NULL;
                        	//if check result failed, notify PC
                        	SEND_ERROR_RSP (BSL_REP_VERIFY_ERROR)
				printf("crc failed");
				printf("\n\n\n%s %s %d\n\n\n", __FILE__, __FUNCTION__, __LINE__);
                        	//return ;
                    	} else {

                        	//It is a complete packet, move to completed list.
                        	packet_ptr->next = NULL;
                        	if (NULL == packet_completed_list) {
                            		packet_completed_list = packet_ptr;
                        	} else {
                            		//added to the tail
                            		tmp_ptr = packet_completed_list;
                            		while (NULL != tmp_ptr->next) {
                                		tmp_ptr = tmp_ptr->next;
                            		}
                            		tmp_ptr->next = packet_ptr;
                        	}
                        	//set to null for next transfer
                        	packet_receiving = NULL;
                        	return ;
                    	}//if (0 != crc)
                } else {//if (HDLC_FLAG == ch)
			if (HDLC_ESCAPE == ch) {
                        	ch = gFdlUsedChannel->GetChar(gFdlUsedChannel);
                        	ch = ch ^ HDLC_ESCAPE_MASK;

				*(pdata + packet_ptr->data_size) = ch;
                    		packet_ptr->data_size += 1;

                    	} else {
				*(pdata + packet_ptr->data_size) = ch;
                    		packet_ptr->data_size += 1;
				nonhdlc_size = 0;
				nonhdlc_size = receive_nonhdlc_data((unsigned char *)&packet_ptr->body, packet_ptr->data_size);
				packet_ptr->data_size += nonhdlc_size;
			}
                }
		/////////////////////////////////////////////////////////////
#else
                if (HDLC_FLAG == ch)
                {
                    packet_ptr->pkt_state = PKT_RECV;
                    // check the packet. CRC should be 0
		if (1 == act_as_romcode)
			crc = crc_16_l_calc((unsigned short *)&packet_ptr->body, packet_ptr->data_size);
		else
			crc = frm_chk ( (unsigned short *) &packet_ptr->body, packet_ptr->data_size);

			if (0 != crc) {
				printf("\n\n\n%s %s %d\n\n\n", __FILE__, __FUNCTION__, __LINE__);
				printf("Richard Feng mask crc check");
				printf("\n\n\n%s %s %d\n\n\n", __FILE__, __FUNCTION__, __LINE__);
				crc = 0;
			}

                    if (0 != crc)
                    {
                        // Verify error, reject this packet.
                        dl_free_packet (packet_ptr);
                        packet_receiving = NULL;
                        // if check result failed, notify PC
                        SEND_ERROR_RSP (BSL_REP_VERIFY_ERROR)
                        //sio_trace( "crc failed" );
                        //return ;
                    }
                    else
                    {


                        // It is a complete packet, move to completed list.
                        packet_ptr->next = NULL;

                        if (NULL == packet_completed_list)
                        {
				packet_completed_list       = packet_ptr;
                        }
                        else
                        {
                            // added to the tail
                            tmp_ptr = packet_completed_list;

                            while (NULL != tmp_ptr->next)
                            {
                                tmp_ptr = tmp_ptr->next;
                            }

                            tmp_ptr->next = packet_ptr;
                        }

                        //set to null for next transfer
                        packet_receiving = NULL;
                        return ;
                    }
                }
                else
                {
                    if (HDLC_ESCAPE == ch)
                    {
                        //ch = sio_get_char();//gFdlUsedChannel->GetChar(gFdlUsedChannel);
                        ch = gFdlUsedChannel->GetChar (gFdlUsedChannel);
                        ch = ch ^ HDLC_ESCAPE_MASK;
                    }

                    * (pdata + packet_ptr->data_size)         = ch;
                    packet_ptr->data_size += 1;
                }
#endif
                break;
            default:
                break;
        }
    }
}

/******************************************************************************
 * write_packet
 ******************************************************************************/
void FDL_WritePacket (const void *buf, int len)
{
    gFdlUsedChannel->Write (gFdlUsedChannel, buf, len);

}


uint32_t  FDL_DataProcess (dl_packet_t *packet_ptr_src, dl_packet_t *packet_ptr_dest)
{
    unsigned short  crc, size;
    int32_t           write_len;  /*orginal length*/
    int32_t           send_len;   /*length after encode*/
    int32_t           i;
    uint8_t           curval;

    uint8_t          *des_ptr = NULL;
    uint8_t          *src_ptr = NULL;
	//static int tmp_cal = 0;

    size = packet_ptr_src->body.size;
    write_len = size + sizeof (unsigned short) + PACKET_HEADER_SIZE;
    src_ptr = (uint8_t *) &packet_ptr_src->body;

    packet_ptr_src->body.size = EndianConv_16 (packet_ptr_src->body.size);
    packet_ptr_src->body.type = EndianConv_16 (packet_ptr_src->body.type);

    /*src CRC calculation*/

	if (1 == act_as_romcode) {
		crc = crc_16_l_calc((const unsigned short *) (& (packet_ptr_src->body)), size + PACKET_HEADER_SIZE);
	} else {
		crc = frm_chk ( (const unsigned short *) (& (packet_ptr_src->body)), size + PACKET_HEADER_SIZE);
		crc = EndianConv_16 (crc);
	}

    packet_ptr_src->body.content[ size ] = (crc >> 8) & 0xFF;
    packet_ptr_src->body.content[ size+1 ] = (crc)    & 0xFF;

    /*******************************************
    *    des data preparation
    ********************************************/

    des_ptr = (uint8_t *) &packet_ptr_dest->body;
    /*head flag*/
    * (des_ptr++) = HDLC_FLAG;
    send_len = 1;

    /*middle part process*/
    for (i = 0; i < write_len; i++)
    {
        curval = * (src_ptr + i);

        if ( (HDLC_FLAG == curval) || (HDLC_ESCAPE == curval))
        {
            * (des_ptr++) = HDLC_ESCAPE;
            * (des_ptr++) = ~HDLC_ESCAPE_MASK & curval;
            send_len++;
        }
        else
        {
            * (des_ptr++) = curval;
        }

        send_len++;
    }

    /*end flag*/
    * (des_ptr++) = HDLC_FLAG;
    send_len++;

    return send_len;
}
/******************************************************************************
 * FDL_SendPacket
 ******************************************************************************/
void dl_send_packet (dl_packet_t *packet_ptr)
{
    int32_t           send_len;   /*length after encode*/
    dl_packet_t       *tmp_packet_ptr = NULL;

    // send a ACK packet to notify PC that we are ready.
    tmp_packet_ptr = FDL_MallocPacket();

    if (NULL == tmp_packet_ptr)
    {

        dl_free_packet (packet_receiving);
        tmp_packet_ptr = FDL_MallocPacket();
    }

    send_len = FDL_DataProcess (packet_ptr, tmp_packet_ptr);

    FDL_WritePacket ( (char *) (& (tmp_packet_ptr->body)), send_len);

    dl_free_packet (tmp_packet_ptr);

}

/******************************************************************************
 * FDL_SendAckPacket_packet
 ******************************************************************************/
void dl_send_ack (dl_cmd_type_t  pkt_type)
{

    unsigned long ack_packet_src[8];
    unsigned long ack_packet_dst[8];
    dl_packet_t *packet_ptr = (dl_packet_t *) ack_packet_src;

    int32_t           send_len;   /*length after encode*/
    dl_packet_t       *tmp_packet_ptr = NULL;

    packet_ptr->body.type = pkt_type;
    packet_ptr->body.size = 0;

    tmp_packet_ptr = (dl_packet_t *) ack_packet_dst;

    send_len = FDL_DataProcess (packet_ptr, tmp_packet_ptr);
	//printf("pkt->body.type=%d\n" , packet_ptr->body.type);
	//printf("before send_len = %d\n" , send_len);
    FDL_WritePacket ( (char *) (& (tmp_packet_ptr->body)), send_len);
	//printf("send_len = %d" , send_len);

}


