#include "call.h"
#
const char *HD = "HD";

int open_t(char *pathname)
{
	int inode_number=0;
	// write your code here.
	int pathname_start_position=0;
	int pathname_length=0;
	char middlefile[5];
	int fd=open(HD,O_RDWR);
	int check=0;
	if(fd==-1)
    {
        inode_number=-1;
        return inode_number;
    }
	inode* ip=malloc(sizeof(inode));
	if(ip==NULL)
    {
        inode_number=-1;
        return inode_number;
    }
    DIR_NODE* data_block=(DIR_NODE*)malloc(BLOCK_SIZE);
    if(data_block==NULL)
    {
        inode_number=-1;
        return inode_number;
    }
    int block_number;
    int file_index=0;



    while(1)
    {
        if(pathname[0]!='/')
        {
            inode_number=-1;
            return inode_number;
        }
        check=lseek(fd,INODE_OFFSET+inode_number*sizeof(inode),SEEK_SET);
        if(check==-1)
        {
            inode_number=-1;
            return inode_number;
        }
        check=read(fd,ip,sizeof(inode));
        if(check==-1)
        {
            inode_number=-1;
            return inode_number;
        }

        if(pathname[pathname_start_position]=='\0')
        {
            //printf("This is the end \n");
            break;
        }

        pathname_start_position++;
        //printf("pathname_start_position in front is %d\n",pathname_start_position);
        while(pathname[pathname_start_position+pathname_length]!='/' && pathname[pathname_start_position+pathname_length]!='\0')
        {
            pathname_length++;
           // printf("pathname_length ++ here\n");
        }
        strncpy(middlefile,pathname+pathname_start_position,pathname_length);
        if(pathname_length<5)
        middlefile[pathname_length]='\0';
        //printf("This is middlefile name\n");
        /*for(int i=0;i<strlen(middlefile);i++)
        {
            printf("%c",middlefile[i]);

        }*/
        //printf("\n");
        pathname_start_position=pathname_start_position+pathname_length;
       // printf("pathname_start_position after ++ is %d\n",pathname_start_position);
        pathname_length=0;


        block_number=ip->direct_blk[0];
        check=lseek(fd,DATA_OFFSET+block_number*BLOCK_SIZE,SEEK_SET);
        if(check==-1)
        {
            inode_number=-1;
            return inode_number;
        }
        check=read(fd,data_block,BLOCK_SIZE);
        if(check==-1)
        {
            inode_number=-1;
            return inode_number;
        }
        //printf("file_num is %d\n",ip->file_num);
        //printf("middlefile strlen is %d\n",strlen(middlefile));
        for(file_index=0;file_index<ip->file_num;file_index++)
        {
            //printf("%s \t %d \n",data_block[file_index].dir,data_block[file_index].inode_number);
            if(strncmp(middlefile,data_block[file_index].dir,strlen(middlefile))==0)
            {

                inode_number=data_block[file_index].inode_number;
               // printf("inode_number is %d here\n",inode_number);
                break;
            }
        }

        if(file_index==ip->file_num)
        {
            inode_number=-1;
            return inode_number;
        }


    }
	return inode_number;
}

int read_t(int inode_number, int offset, void *buf, int count)
{
    int check=0;
    //printf("buf len is %d at the beginning\n",strlen(buf));
    memset(buf,0x00,MAX_FILE_SIZE);
   // printf("buf len is %d after clear\n",strlen(buf));
	int read_bytes;
    if(count<0 || offset<0 || buf==NULL)
    {
       read_bytes=-1;
       return read_bytes;
    }
	// write your code here.
    int file_size=1239081;
	//printf("offset is %d at the beginning\n",offset);
	if(offset>=file_size)
    {
        read_bytes=0;
        return read_bytes;
    }
    if(offset+count>file_size)
    {
        count=file_size-offset;
        //printf("the new count is %d\n",count);
    }
	int fd=open(HD,O_RDWR);
	inode* ip=malloc(sizeof(inode));
	int begina=offset/4096;
	//printf("begina is %d\n",begina);
	int beginb=offset%4096;
	//printf("beginb is %d\n",beginb);
	int enda=(offset+count)/4096;
	//printf("end a is %d\n",enda);
	int endb=(offset+count)%4096;
	//printf("endb is %d\n",endb);
	int datablockusing=begina;
	int datablockleft=enda-begina+1;
	//printf(" datablockusing is %d and datablockleft is %d\n",datablockusing,datablockleft);
	int* blocknumber=(int*)malloc(sizeof(int));
	if(blocknumber==NULL)
    {
        read_bytes=-1;
        return read_bytes;
    }
	int placeforholdoffset=offset;

	//int byteread=0;
	char tmp[4096];



	//printf("after initlization, offset is %d\n",offset);
	check=lseek(fd,INODE_OFFSET+inode_number*sizeof(inode),SEEK_SET);
	if(check==-1)
    {
        read_bytes=-1;
        return read_bytes;
    }
	check=read(fd,ip,sizeof(inode));
    if(check==-1)
    {
        read_bytes=-1;
        return read_bytes;
    }
	//printf("after assign ip offset is %d\n",offset);


	if(datablockleft==enda-begina+1)//the beginning data block
    {
        //printf("enter to the beginning block\n");
        //printf("offset is %d here\n",offset);
         if(datablockusing<2)
        {
            //printf("before assign blocknumber offset is %d\n",offset);
            *blocknumber=ip->direct_blk[datablockusing];
            offset=placeforholdoffset;
            //printf("after assign blocknumber offset is %d\n",offset);
            //printf("block number is %d here\n",*blocknumber);

        }


        else if(datablockusing>=2)
        {
            check=lseek(fd,DATA_OFFSET+(ip->indirect_blk)*BLOCK_SIZE+(datablockusing-2)*4,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            check=read(fd,blocknumber,4);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            //printf("block number is %d here\n",*blocknumber);
        }
            //printf("offset is %d\n",offset);
            check=lseek(fd,DATA_OFFSET+ (*blocknumber)*BLOCK_SIZE+offset-4096*datablockusing,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            //printf("count is %d before reading data from  data block\n",count);
            if(offset+count<(datablockusing+1)*4096)
            {
                //printf("buf len is %d before read data\n",strlen(buf));
               check=read(fd,buf,count);
                if(check==-1)
                {
                    read_bytes=-1;
                    return read_bytes;
                }
               //byteread=byteread+count;
               //printf("buf len is %d after the first read data\n",strlen(buf));
               //printf("it begin and end at the same block\n");
            }

            else if(offset+count>=(datablockusing+1)*4096)
            {
                check=read(fd,buf,(datablockusing+1)*4096-offset);
                if(check==-1)
                {
                    read_bytes=-1;
                    return read_bytes;
                }
                //byteread=byteread+(datablockusing+1)*4096-offset;
                //printf("buf len is %d after the first data block\n",strlen((buf)));
                //printf("it begin and end at different block\n");
            }

            datablockusing++;
            datablockleft--;
    }

    //printf("before enter to loop datablockusing is %d and data blockleft is %d\n",datablockusing,datablockleft);
    while((datablockleft<(enda-begina+1))&&(datablockleft>1))
    {

        if(datablockusing==1)
        {
            *blocknumber=ip->direct_blk[datablockusing];
        }

        else if(datablockusing>=2)
        {
            check=lseek(fd,DATA_OFFSET+(ip->indirect_blk)*BLOCK_SIZE+(datablockusing-2)*4,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            check=read(fd,blocknumber,4);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
        }
            check=lseek(fd,DATA_OFFSET+ (*blocknumber)*BLOCK_SIZE,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            //lseek(buf,byteread,SEEK_SET);
            check=read(fd,tmp,BLOCK_SIZE);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            strcat(buf,tmp);
            memset(tmp,0x00,4096);

            //byteread=byteread+BLOCK_SIZE;
            datablockusing++;
            datablockleft--;
           // printf("before enter to loop datablockusing is %d and data blockleft is %d\n",datablockusing,datablockleft);
            //printf("buf strlen is %d after while loop\n",strlen((buf)));
    }


    if(datablockleft==1)
    {
       // printf("enter to the final block\n");
        //printf("before read the final data block strlen(buf) is %d\n",strlen(buf));
       // printf("datablockusing is %d\n",datablockusing);
        if(datablockusing==1)
        {
            *blocknumber=ip->direct_blk[datablockusing];
        }

        else if(datablockusing>=2)
        {
            check=lseek(fd,DATA_OFFSET+(ip->indirect_blk)*BLOCK_SIZE+(datablockusing-2)*4,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
            check=read(fd,blocknumber,4);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
        }
            check=lseek(fd,DATA_OFFSET+(*blocknumber)*BLOCK_SIZE,SEEK_SET);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
           // lseek(buf,byteread,SEEK_SET);
          // printf("endb is %d at the last block\n",endb);
           //printf("strlen(buf) is %d here\n",strlen(buf));
            check=read(fd,tmp,endb);
            if(check==-1)
            {
                read_bytes=-1;
                return read_bytes;
            }
           // printf("strlen(tmp) is %d\n",strlen(tmp));
            strcat(buf,tmp);
            memset(tmp,0x00,4096);
           // byteread=byteread+endb;
            datablockusing++;
            datablockleft--;
    }


    read_bytes=strlen(buf);





	return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only “open_t()” and "read_t()" are allowed to call these auxiliary functions.
