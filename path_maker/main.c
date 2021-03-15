#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <fcntl.h>

void komut_atla(char *pointer,int* indx);
char* isKeyword(char string[],int uzunluk);

int main()
{
    char* cwd[PATH_MAX]; // Current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }
    char dosya_adi[20];
    printf("\n");
    printf("Dosya adini giriniz (dosya) : ");
    scanf("%s",dosya_adi);
    printf("\n");
    //Sonuna .pmk ve .txt ekleri eklendi
    strcat(dosya_adi,".pmk");
    strcat(dosya_adi,".txt");


    FILE *file; // Dosya okuma işlemi.
    file = fopen(dosya_adi,"r");
    if((file = fopen(dosya_adi,"r"))== NULL)
    {
        printf("Dosya okunamadi. \n");
        exit(0);
    }

    char ch;
    char* pointer;


    int uzunluk =0;
    while((ch = fgetc(file)) != EOF)
    {
        uzunluk++;
    }


    // Dosyadaki karakter sayısını buldu
    rewind(file);
    pointer = (char*)malloc(uzunluk*sizeof(char));

    if(pointer== NULL)
    {
        printf("Memory error. \n");
        exit(0);
    }


    int sayac =0;
    while((ch = fgetc(file)) != EOF)
    {
        pointer[sayac] = ch;
        sayac++;
    }

    for(int indx=0; indx<uzunluk;indx++)
    {
        char *Control = (char*)malloc(50* sizeof(char)); // if, ifnot, go, make, delete
        char *directory_name = (char*)malloc(100* sizeof(char)); //<> isaretleri arası

        //En başta comment varsa es geçer.
        if(pointer[indx] == '[')
        {
            while(pointer[indx] != ']')
            {
            indx++;
            }
            indx++;
        }

        //Control structure lar ve commandlar ın hepsi harfle başladığı için ilk harf görüşünde if içerisine girer.

        if(isalpha(pointer[indx]) !=0)
        {

            int counter =0;
            while( isalpha(pointer[indx]) !=0 || (isdigit((char)pointer[indx])== 1) )
            {
                Control[counter] = pointer[indx];
                counter++;
                indx++;
            }

            //if, ifnot, go, make, delete yapılarından herhangi birinden sonra comment  gelirse es geçecek comment bitene kadar.

            if(pointer[indx] == '[' || (pointer[indx] == ' ' && pointer[indx+1] == '[' ))
            {
                while(pointer[indx] != ']')
                {
                indx++;
                }
                indx++; // Bosluk indeksinde
            }
            //Directory kurallara uygun mu kontrol.

            if(pointer[indx+1] =='<')
            {
                indx+=2;
                if(pointer[indx] == '/')
                {
                    printf("%d indeksinde Baslangicta / isareti kullanilmis.",indx);
                    break;
                }
                int counter = 0; // Counter sıfırlandı.

                while(pointer[indx] != '>')
                {
                    //Bosluk atlandi
                    if(pointer[indx] == ' ')
                    {
                        indx++;
                        continue;
                    }

                    if(pointer[indx] == '/' && pointer[indx+1] == '>')
                    {
                        printf("%d indeksinde Directory bitisinde / isareti kullanilmis.",indx);
                        return 0;
                    }
                    directory_name[counter] = pointer[indx];
                    counter++;
                    indx++;
                }
                directory_name[counter] = '\0';
            }
            else
            {
                printf("Bilinmeyen hata\n");
                return 0;
            }

            if(isKeyword(Control,counter) != "-1")
            {
                char keyword[counter+1];
                for(int i=0;i<counter+1;i++)
                {
                    keyword[i] = isKeyword(Control,counter)[i];
                }
                if(strcmp(keyword,"if") == 0)
                {
                    printf("***************************if komutuna girdi *************************************\n");
                    // eski cwd tutuldu
                    char tempPath[PATH_MAX];
                    strcpy(tempPath,cwd);
                    //cwd yi directory name lerine göre güncellemek için directory_name tokenlerine ayrıldı.
                    char *tokenCheck = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck,directory_name);
                    char* token;
                    token = strtok(tokenCheck,"/");
                    int sayacForToken = 0;
                    getcwd(cwd,sizeof(cwd));
                    while(token != NULL)
                    {
                        if(strcmp(token,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {
                            strcat(cwd,"/"); // Tokenlerin başına / koyarak onların path olarak kullanmak için hazırlıyoruz.
                            strcat(cwd,token);
                        }
                        token = strtok(NULL,"/");
                    }
                    if(chdir(cwd) != 0)
                        komut_atla(pointer,&indx); //directory yoksa komut bloğu ya da satırı atlanmalı

                    //cwd alındı
                    getcwd(cwd,sizeof(cwd));

                    /*
                    tempPath2 değişkeni cwd nin modifiye edilmeden önceki halini tutuyordu. if kontrol yapısı cwd yi değiştirmeyeceği
                    için işlemlerimiz bittiğinde cwd yi eski haline getirmek için kullanıldı.
                    */
                    if(chdir(tempPath) != 0)
                        perror("chdir() error.");

                    getcwd(cwd,sizeof(cwd));
                    printf("Directory : %s\n",directory_name);
                    printf("Cikis cwd : %s\n",cwd);
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"ifnot") == 0)
                {
                    printf("***************************ifnot komutuna girdi *************************************\n");
                    // İf kısmından kopyalandı
                    // eski cwd tutuldu
                    char tempPath[PATH_MAX];
                    strcpy(tempPath,cwd);
                    char *tokenCheck = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck,directory_name);
                    char* token;
                    token = strtok(tokenCheck,"/");
                    getcwd(cwd,sizeof(cwd));
                    while(token != NULL)
                    {
                        if(strcmp(token,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {
                            strcat(cwd,"/");
                            strcat(cwd,token);
                        }
                        token = strtok(NULL,"/");
                    }
                    if(chdir(cwd) != 0)
                        komut_atla(pointer,&indx); //directory varsa komut bloğu/satırı atlanmalı
                    getcwd(cwd,sizeof(cwd));
                    if(chdir(tempPath) != 0)
                        perror("chdir() error.");

                    getcwd(cwd,sizeof(cwd));
                    printf("Directory : %s\n",directory_name);
                    printf("Cikis cwd : %s\n",cwd);
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"go") == 0)
                {
                    printf("***************************go komutuna girdi *************************************\n");
                    char *tokenCheck = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck,directory_name);
                    char* token;
                    token = strtok(tokenCheck,"/");
                    getcwd(cwd,sizeof(cwd));
                    while(token != NULL)
                    {
                        if(strcmp(token,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {
                            strcat(cwd,"/");
                            strcat(cwd,token);

                            if(chdir(cwd) != 0) //directory'e gidilemezse hata vermeli
                            {
                                printf("HATA: go commandinde verilen directory'e gidilemiyor.\n");
                                return 0;
                            }

                        }
                        token = strtok(NULL,"/");
                    }
                    getcwd(cwd,sizeof(cwd));
                    printf("Directory : %s\n",directory_name);
                    printf("Cikis cwd : %s\n",cwd);
                    printf("***********************************************************************");
                    printf("\n");
                }

                else if(strcmp(keyword,"make") == 0)
                {
                    printf("***************************make komutuna girdi *************************************\n");
                    // eski cwd tutuldu
                    char tempPath[PATH_MAX];
                    strcpy(tempPath,cwd);
                    char *tokenCheck = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck,directory_name);
                    char* token;
                    token = strtok(tokenCheck,"/");
                    int sayacForToken = 0;
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı gözlemlendi.
                        Örneğin chdir("/A") yaptığımızda C:/A oldu
                    */
                    bool stateForMake = true;
                    while(token != NULL)
                    {
                        if(strcmp(token,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));
                        }
                        else
                        {

                            strcat(cwd,"/");
                            strcat(cwd,token);
                            //Creating Directory
                            /*
                                Directory halihazırda varsa path i o directorye taşıyor.
                                Yoksa oluşturmaya çalışıyor: Eğer oluşturamazsa directorynin olduğu anlamına geliyor. StateForMake durum değiştirmezse
                                directory olduğu biliniyor

                            */
                            if(chdir(cwd)==0)
                            {
                                getcwd(cwd,sizeof(cwd));
                            }
                            else
                            {
                                stateForMake = false;
                                int status;
                                status = mkdir(cwd);

                                if(status != 0)
                                {
                                    printf("make commandinde hata\n");
                                    return 0;
                                }
                            }
                        }
                        token = strtok(NULL,"/");
                    }

                    if(stateForMake == true)
                    {
                        int status;
                        status = mkdir(cwd); //mkdir hatasız çalıştığında sıfır değerini döndürür
                        if(status != 0)
                        {
                            printf("HATA: make command i Directory olusturamadi. Ayni isimde directory olabilir.\n");
                        }
                    }

                    getcwd(cwd,sizeof(cwd));
                    // Current working directory nin değişmesi önlendi.
                    if(chdir(tempPath) != 0)
                        perror("chdir() error.");
                    getcwd(cwd,sizeof(cwd));
                    //printf("< > isaretleri arasindaki Directory : %s\n",directory_name);
                    printf("Directory : %s\n",directory_name);
                    printf("Cikis cwd : %s\n",cwd);
                    printf("***********************************************************************");
                    printf("\n");

                }
                else if(strcmp(keyword,"delete") == 0)
                {
                    printf("***************************delete komutuna girdi *************************************\n");


                    // eski cwd tutuldu
                    char tempPath[PATH_MAX];
                    strcpy(tempPath,cwd);

                    char tempPath2[PATH_MAX];
                    getcwd(cwd,sizeof(cwd));
                    strcpy(tempPath2,cwd);


                    //cwd yi directory name lerine göre güncellemek için directory_name tokenlerine ayrıldı.
                    printf("Directory : %s\n",directory_name);
                    char *tokenCheck = (char*)malloc(100* sizeof(char));
                    strcpy(tokenCheck,directory_name);
                    char* token;
                    token = strtok(tokenCheck,"/");;
                    int sayacForToken = 0;
                    getcwd(cwd,sizeof(cwd));
                    /*
                    Tokenlemeden önce sürekli cwd alındı. Bu yapılmadığında current cwd C: olarak kaldığı zamanlar oldu.
                        Örneğin chdir("/A") yaptığımızda C:/A oldu
                    */
                    bool stateForDelete = true;
                    while(token != NULL)
                    {
                        if(strcmp(token,"*")==0)
                        {
                            chdir("..");
                            getcwd(cwd,sizeof(cwd));

                        }
                        else
                        {
                            strcat(cwd,"/"); // Tokenlerin başına / koyarak onların path olarak kullanmak için hazırlıyoruz.
                            strcat(cwd,token);

                            if(chdir(cwd)==0)
                            {
                                getcwd(cwd,sizeof(cwd));
                            }
                            else
                            {
                                stateForDelete = false;

                            }
                        }
                        token = strtok(NULL,"/");
                    }

                    if(stateForDelete == false)
                    {
                        printf("Silinmesi istenen directory bulunamadi.\n");
                        return 0;

                    }
                    else if(stateForDelete == true)
                    {
                        int status;
                        status = remove_directory(cwd);
                        rmdir(cwd);

                        if( status == -1 )
                        printf("Silme islemi basariyla gerceklesti\n");
                        else {
                        printf("Silme isleminde beklenmeyen hata!\n");
                        }
                    }

                    //Her chdir çalıştığında current working directory (cwd) değişir ve bu değişimi tekrar cwd ye getcwd metodu ile atandı.
                    getcwd(cwd,sizeof(cwd));

                    /*
                    tempPath2 değişkeni cwd nin modifiye edilmeden önceki halini tutuyordu. if kontrol yapısı cwd yi değiştirmeyeceği
                    için işlemlerimiz bittiğinde cwd yi eski haline getirmek için kullanıldı.
                    */
                    if(chdir(tempPath) != 0)
                        perror("chdir() error.");
                    getcwd(cwd,sizeof(cwd));
                    printf("Cikis cwd : %s\n",cwd);
                    printf("***********************************************************************\n");
                }
            }
        }
        free(Control);
        free(directory_name);
    }
    free(dosya_adi);
    return 0;
}


//Okunan stringlerin keyword olup olmadığına baktığımız metot.

char* isKeyword(char stringToken[],int uzunluk) // Keywordler case sensitive olmalı
{
    char keywordler[5][8] =
    {
         {'i','f','\0'}, {'i','f','n','o','t','\0'}, {'m','a','k','e','\0'}, {'g','o','\0'}, {'d','e','l','e','t','e','\0'}
    };

    int i;
    for(i=0;i<5;i++)
    {

        if(strncmp(keywordler[i],stringToken,uzunluk)==0)
        {
            char *s = (char*)(calloc(8,sizeof(char)));
            strcpy(s,keywordler[i]);
            return s;
        }
    }
    return "-1";
}

// Şartlar sağlanmazsa kod bloğunun / satırının atlanmasını sağlayan metot.
void komut_atla(char *pointer,int* indx)
{
    printf("Komut atlandi\n");
    int temp = *indx;
    if(pointer[temp+2] == '{' || pointer[temp+3] == '}')
    {
        while(pointer[temp] != '}')
        {
            temp++;
        }
        *indx = temp;
    }
    else
    {
        while(pointer[temp] != '}')
        {
            temp++;
        }
        *indx = temp;

    }
}
/* silme komutunun icine ornegin furkansamet/d verildi ve d'nin icinde baska dosya ve klasorler var. silme islemi d'nin icindekileri siler d'yi degil */
int remove_directory(const char *path) { //silme komutu
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          /* "." ve ".." leri es geciyor */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r; // -1 ise silme islemi gerceklesi degilse gerceklesmedi
}

