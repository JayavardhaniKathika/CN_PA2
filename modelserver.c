#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

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
    typedef struct packet{
        uint16_t startPacketId; //2bytes
        uint8_t clientId;  //1 byte   Maximum 0XFF (255 Decimal)
        uint16_t permission;   //2bytes
        uint8_t segmentno;  //1 byte
        uint8_t length;  //1byte   Maximum 0XFF (255 Decimal)
        uint8_t technology;  //1 byte
        uint32_t  sourceSubscriberNumber; //4bytes   Maximum 0XFFFFFFFF (4294967295 Decimal)
        uint16_t endPacketId; //2bytes
    }packet;
#pragma pack(pop)

void printPacketDetails(packet pp){
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

    packet receivedPacket;
    packet sendPacket;
    sendPacket.startPacketId=START_OF_PACKET_IDENTIFIER;
    sendPacket.endPacketId=END_OF_PACKET_IDENTIFIER;



    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    
    int client_struct_length = sizeof(client_addr);
    
    
    // Create UDP socket:
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
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
    
    printf("Listening for incoming messages...\n\n");
    
   
  

    
    while(1){

        // Receive client's message:
        if (recvfrom(socket_desc, &receivedPacket, sizeof(receivedPacket), 0,(struct sockaddr*)&client_addr, (socklen_t *)&client_struct_length) < 0){
            printf("Couldn't receive\n");
            return -1;
        }
        printf("----Received message from IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


        printPacketDetails(receivedPacket);

        
        sendPacket.clientId=receivedPacket.clientId;
        sendPacket.segmentno=receivedPacket.segmentno;
        sendPacket.length=receivedPacket.length;
        sendPacket.technology=receivedPacket.technology;
        sendPacket.sourceSubscriberNumber=receivedPacket.sourceSubscriberNumber;




        char subscriber[20];
        char tech[20];
        char pay[3];
        uint32_t snumber;
        uint8_t techNo;
        int paid=0;
        int found=0;
    

        fp = fopen("Verification_Database.txt", "r");
        if(sendPacket.sourceSubscriberNumber==(uint32_t)1111111111){

        }
        else{
            while(fgets(buff, 255, (FILE*)fp)!=NULL){
            // Extract the first token
            char *token = strtok(buff, " ");
            strcpy(subscriber,token);
            snumber= (uint32_t) atoi(subscriber);
                if(snumber==receivedPacket.sourceSubscriberNumber){  
                 // extract all other tokens
                    found=1;
                    token = strtok(NULL, " ");
                    strcpy(tech,token);
                    token = strtok(NULL, " ");
                    strcpy(pay,token);
                    techNo=(uint8_t) atoi (tech);
                    paid = atoi(pay);
                    break;
                }
            
            }
            if(found==1){
                if(paid==0){
                    sendPacket.permission=NOT_PAID;
                    if (sendto(socket_desc, &sendPacket, sizeof(sendPacket), 0,(struct sockaddr*)&client_addr, client_struct_length) < 0){
                        printf("Can't send\n");
                        return -1;
                    }
                }
                else if(receivedPacket.technology<0X02 || receivedPacket.technology>0X05){
                    sendPacket.permission=NOT_EXIST;
                    if (sendto(socket_desc, &sendPacket, sizeof(sendPacket), 0,(struct sockaddr*)&client_addr, client_struct_length) < 0){
                        printf("Can't send\n");
                        return -1;
                    }
                }
                else{
                    sendPacket.permission=ACCESS_OK;
                    if (sendto(socket_desc, &sendPacket, sizeof(sendPacket), 0,(struct sockaddr*)&client_addr, client_struct_length) < 0){
                        printf("Can't send\n");
                        return -1;
                    }

                }

            }
            else{
                sendPacket.permission=NOT_EXIST;
                if (sendto(socket_desc, &sendPacket, sizeof(sendPacket), 0,(struct sockaddr*)&client_addr, client_struct_length) < 0){
                        printf("Can't send\n");
                        return -1;
                }
            }
            }
    
        
          
        fclose(fp);  
    }
    
    
    

    
    return 0;
}
