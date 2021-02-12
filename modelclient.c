#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <time.h>  
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>


#define PORT_NO 2000
#define START_OF_PACKET_IDENTIFIER 0XFFFF
#define END_OF_PACKET_IDENTIFIER 0XFFFF

#define ACCESS_PERMISSION 0XFFF8
#define NOT_PAID 0XFFF9
#define NOT_EXIST 0XFFFA
#define ACCESS_OK 0XFFFB

// Technology
#define SECOND_GENERATION 0X02
#define THIRD_GENERATION 0X03
#define FOURTH_GENERATION 0X04
#define FIFTH_GENERATION 0X05

#pragma pack(push,1)
    //packet format
    typedef struct permissionPacket{
        uint16_t startPacketId; //2bytes
        uint8_t clientId;  //1 byte   Maximum 0XFF (255 Decimal)
        uint16_t permission;   //2bytes
        uint8_t segmentno;  //1 byte
        uint8_t length;  //1byte   Maximum 0XFF (255 Decimal)
        uint8_t technology;  //1 byte
        uint32_t  sourceSubscriberNumber; //4bytes   Maximum 0XFFFFFFFF (4294967295 Decimal)
        uint16_t endPacketId; //2bytes
    }permissionPacket;

#pragma pack(pop)


void printPacketDetails(permissionPacket pp){
    
    printf("---------------------------------------------\n");
    printf("StartPacketId %x\n",pp.startPacketId);
    printf("ClientId %x\n",pp.clientId);
    printf("Permission %x\n",pp.permission);
    printf("SegmentNumber %u\n",pp.segmentno);
    printf("Length %x\n",pp.length);
    printf("Technology %u\n",pp.technology);
    printf("SourceSubscriberNumber %u\n",pp.sourceSubscriberNumber);
    printf("EndPacketId %x\n",pp.endPacketId);
    printf("-------------------------------------------------------\n");
}




int main(void){
    FILE *fp;
    char buff[255];

    uint8_t clientId=0XFA;
    uint8_t segmentnumber=0x1;

    permissionPacket pp;
    pp.startPacketId=START_OF_PACKET_IDENTIFIER;
    pp.clientId=clientId;
    pp.permission=ACCESS_PERMISSION;
    pp.segmentno=segmentnumber;
    //pp.technology=0X03;
    //pp.sourceSubscriberNumber=0XFFFFFFFF;
    pp.length=sizeof(pp.technology)+sizeof(pp.sourceSubscriberNumber);
    pp.endPacketId=END_OF_PACKET_IDENTIFIER; 
    

    int socket_desc;
    struct sockaddr_in server_addr;
    int server_struct_length = sizeof(server_addr);
    

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NO);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



    fp = fopen("Subscriber_Data.txt", "r");

    while(fgets(buff, 255, (FILE*)fp)!=NULL){
        char *token = strtok(buff, " ");
        char subscriber[20];
        strcpy(subscriber,token);
        pp.sourceSubscriberNumber= (uint32_t) atoi(subscriber);

        token = strtok(NULL, " ");
        char tech[20];
        strcpy(tech,token);
        pp.technology=(uint8_t) atoi(tech);

        int retryCounter=0;
        fd_set rfds;
        struct timeval tv;
        int retval;
        permissionPacket buffer;

        
        while(retryCounter<3){

            // Send the message to server:
            if(sendto(socket_desc, &pp, sizeof(pp), 0,
            (struct sockaddr*)&server_addr, server_struct_length) < 0){
                printf("Unable to send message\n");
                return -1;
            }
            else{
                printf("Sending Packet\n\n");
                //printPacketDetails(pp);
                
            } 

            /* Watch stdin (fd 0) to see when it has input. */
            FD_ZERO(&rfds);
            FD_SET(socket_desc, &rfds);
            tv.tv_sec = 3;
            tv.tv_usec = 0;
           
            retval=select(socket_desc+1, &rfds, NULL, NULL, &tv);
            
            
            //Receive the server's response:
            
            if(retval){
                recvfrom(socket_desc, &buffer, sizeof(permissionPacket), 0,
                    (struct sockaddr*)&server_addr, (socklen_t *)&server_struct_length);
                if(buffer.permission==NOT_PAID){
                    printf("Error: Subscriber did not pay the bill\n");
                }
                else if(buffer.permission==NOT_EXIST){
                    printf("Error: Subscriber does not Exist on database\n");
                }
                else{
                    printf("Subscriber permitted to access the network\n");
                }
                printPacketDetails(buffer);
                
                break;
            }
            else{
                retryCounter++;
                if(retryCounter<3){
                    printf("\nError: Server didnot respond Resending the packet\n");
                }
                
                
            }

        }
        if(retryCounter==3){
            printf("\nError: Server does not respond\n");
            break;
        }

        
        
    } 
    fclose(fp);  
    // Close the socket:
    close(socket_desc);
    
    return 0;
}

