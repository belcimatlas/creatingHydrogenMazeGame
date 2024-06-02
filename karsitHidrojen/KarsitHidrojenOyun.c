#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#define MAX 20
struct Kullanici {        //Kullan�c� bilgileri structure'da tutuluyor
	char isim[50];
	char soyisim[50];
	char kullaniciAdi[50];
	char sifre[50];
	int skor;
}kullanici;

struct Kullanici kullaniciListesi[MAX];
int kullaniciSayisi=0;
char parcaciklar[50];
int parcacikSayisi=0;

int hesabaGirisMenusu();
int oyunIciMenu();
void printOyunKurallari();
int hesapOlustur(struct Kullanici *kullanici);
int kullaniciAdiKontrol(char *kullaniciAdi);
int login(struct Kullanici *kullanici);
void binaryKayit();
void readFromFile(char** labirent, char *fileName, int N, int M);
char** labirentYerAc(int N, int M);
void freeLabirent(char** labirent, int N);
void printLabirent(char** labirent, int N, int M);
void baslangicKonumuBul(char** labirent, int N, int M, int *x, int *y);
int karsitHidrojenBul();
void hamleYap(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi);
void hamleAl(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi, int oyunModu);
void otomatikYon(int *yon);
void printParcacik();
int skorHesapla(int girisYapanKullanici);
void enYuksekSkorlar();

int hesabaGirisMenusu() {     //Kullan�c�n�n oyunu a�t���nda g�rd��� ilk men�.
	int hesabaGirisSecim;     //Hesap olu�turma, login veya ��k�� i�lemlerini se�tirir.
	printf("Karsit Madde Labirenti'ne hosgeldiniz!\nHesap olusturmak icin 1\nLogin icin 2\nCikis yapmak icin 3 giriniz.\nSecim: ");
	scanf("%d", &hesabaGirisSecim);
	return hesabaGirisSecim;
}

int oyunIciMenu() {           //Kullan�c�n�n hesap olu�turdukttan veya login olduktan sonra g�rd��� men�.
	int oyunIciSecim;         //En y�ksek 5 skoru g�r�nt�leme, oyun kurallar�n� g�r�nt�leme, oyuna ba�lama veya kullan�c� de�i�tirme i�lemlerini se�tirir.
	printf("\nEn yuksek skorlar ve sahiplerini goruntulemek icin 1\nOyun kurallari icin 2\nOyuna baslamak icin 3\n"
		"Kullanici degistirmek veya cikis yapmak icin 4 giriniz.\nSecim: ");
	scanf("%d", &oyunIciSecim);
	return oyunIciSecim;
}

void printOyunKurallari() {   //Oyun kurallar�n� yazd�r�r.
	printf("\n------OYUN KURALLARI------"
	   "\nKarsit Madde Labirenti, sececeginiz veya yukleyecginiz labirent haritalari uzerinde oynanir."
       "\nLabirentteki yollarda atomalti parcaciklar vardir. "
       "\n'x' ile belirtilen konum kullanicinin ilk konumunu belirtir."
	   "\nKullanici yon tuslariyla haritada gezerek atomalti parcaciklari toplar."
	   "\n'1'ler duvar, '0'lar yollari temsil eder. Duvara veya labirent disina hamle yapilirsa konum degismez."
	   "\nIstenildiginde 'esc' tusu ile oyun sonlandirilir. Aksi takdirde 'C'ye ulasilana kadar devam eder."
	   "\n'K'ler kara deliklerdir. Ustlerinden gecilirse oyun sonlandirilir."
	   "\nOyunun amaci labirentte gezip 'p+' ve 'e-'leri toplamaktir. Bu iki parcacik birlestiginde karsit hidrojen uretilir."
	   "\nBir parcacik, karsit isaretlisi ile bir araya gelirse birbirlerini yok ederler."
	   "\nHamle yapilmaya baslandiktan 30 saniye sonra oyun sonlanir.\n\n"
	   );
}

int hesapOlustur(struct Kullanici *kullanici){                 //Hesap olu�turur ve struct'a kaydeder.
	printf("\nHesap Olusturma\n");
	printf("Isim: ");
	scanf("%s", kullanici->isim);
	printf("Soyisim: ");
	scanf("%s", kullanici->soyisim);
	do{
		printf("Kullanici Adi: ");
		scanf("%s", kullanici->kullaniciAdi);
		if(kullaniciAdiKontrol(kullanici->kullaniciAdi)){     //Kullan�c� ad�n�n daha �nceden al�n�p al�nmad���n� kontrol eden kullaniciAdiKontrol fonksiyonunu �a��r�r.
			printf("Kullanici adi �nceden alinmis. Yeni bir kullanici adi girin.\n");
		}
	}while(kullaniciAdiKontrol(kullanici->kullaniciAdi));
	printf("Sifre: ");
	scanf("%s", kullanici->sifre);
	kullanici->skor=0;
	kullaniciListesi[kullaniciSayisi]= *kullanici;
	kullaniciSayisi++;
	return kullaniciSayisi-1;                                 //Fonksiyon parametre olarak ald��� kullan�c�n�n, kullan�c�lar�n tutldu�u listede ka��nc� s�rada oldu�unu d�nd�r�r. 
}

int kullaniciAdiKontrol(char *kullaniciAdi){                  //Kullan�c� ad�n�n daha �nce al�n�p al�nmad���n� kontrol eder.
	int i;
	for(i=0; i<kullaniciSayisi; i++){
		if(strcmp(kullaniciAdi, kullaniciListesi[i].kullaniciAdi)==0){
			return 1;                                        //E�er kullan�c� ad� �nceden al�nm��sa 1 d�nd�r�r. Al�nmam��sa o d�nd�r�r.
		}
	}
	return 0;
}

int login(struct Kullanici *kullanici){                     //Login i�lemini ger�ekle�tirir.
	char loginAd[50], loginSifre[50];
	int i, kullaniciKontrol;
	
	printf("Kullanici adi: ");
	scanf("%s", loginAd);
	printf("Sifre: ");
	scanf("%s", loginSifre);
	for(i=0; i<kullaniciSayisi; i++){                      //T�m kullan�c�lar gezilerek login olmak istenen kullan�c� aran�r.
		kullaniciKontrol=0;
		if(strcmp(kullaniciListesi[i].kullaniciAdi, loginAd) == 0){
			kullaniciKontrol++;
		}
		if(strcmp(kullaniciListesi[i].sifre, loginSifre) == 0){
			kullaniciKontrol++;
		}
		if(kullaniciKontrol==2){
			printf("Login basarili!\nHosgeldin %s", kullaniciListesi[i].kullaniciAdi);    //Kullan�c� ad� ve �ifre do�ruysa login ger�ekle�tirilmi� olur ve kullan�c�n�n kullan�c� listesindeki s�ras�n� d�nd�r�r.
			return i;
		}
	}
	printf("\nKullanici bulunamadi. Yeniden deneyin.\n");                     //Kullan�c� ad� veya �ifre yanl��sa uyar� verir. 
	return -1;
		
}

void binaryKayit(){                              //Hesap olu�turuldu�unda bilgiler binary olarak kullaniciData.txt dosyas�na kaydedilir. 
	struct Kullanici kullanici;
	FILE *bfptr = fopen("kullaniciData.txt", "wb");
	if (bfptr == NULL) {
        printf("Binary veri tutulacak dosya acilamadi.\n");
        exit(0);
    }
    fwrite(&kullanici, sizeof(struct Kullanici), 1, bfptr);
    fclose(bfptr);
}
 
void readFromFile(char** labirent, char *fileName, int N, int M) {    //Kullan�c�dan ad� al�nan dosyadaki matris bilgisi okunur
                                                                      //ve dinamik haf�za ile yer a��lan labirent matrisine okunan bilgiler yaz�l�r.
	int i, j;
	int elemanSayisi;
    FILE* fptr = fopen(fileName, "r");                                //Dosya okuma modunda a��l�r.

    if (fptr==NULL) {
        printf("Dosya acilamadi.\n");
        exit(1);
    }
    
    for (i=0; i<N; i++) {
        for (j=0; j<M; j++) {
            if (fscanf(fptr, "%c", &labirent[i][j]) != 1) {           //Dosyadan okunup labirente yaz�l�r.
                printf("Dosya okuma hatasi.\n");
                exit(1);
            }
        }
    }
    fclose(fptr);
}

char** labirentYerAc(int N, int M){                 //Kullan�c�dan al�nan matris boyutlar�na g�re labirent i�in dimanik olarak haf�zada yer a��l�r.
	int i;
	char** labirent=(char**)malloc(N * sizeof(char*));
	for(i=0; i<N; i++) {
        labirent[i] = (char*)malloc(M * sizeof(char));
    }
    return labirent;
}

void freeLabirent(char** labirent, int N) {         //Oyun sonland���nda matris i�in ayr�lan yeri serbest b�rak�r.
    int i;
    for(i = 0; i < N; i++) {
        free(labirent[i]);
    }
    free(labirent);
}


void printLabirent(char** labirent, int N, int M) {    //Labirent matrisini ekrana yazd�r�r.
	int i,j;
    for(i=0; i<N; i++) {
        for(j=0; j<M; j++) {
            printf("%c\t", labirent[i][j]);
        }
    	printf("\n");
    }
}

void baslangicKonumuBul(char** labirent, int N, int M, int *x, int *y){   //Oyunu ba�latmak i�in labirent i�inde gezip X'i yani ba�lang�� konumunu bulur.
																		  //Buldu�u koordinatlar� x ve y olarak tutar.
	int i, j;
	for(i=0; i<N; i++) {
        for(j=0; j<M; j++) {
            if(labirent[i][j]=='X'){
            	*x=i;
            	*y=j;
			}
        }
    }
}

int karsitHidrojenBul() {               //e+(E) ve p-(p) birle�imi kar��t hidrojen olu�turur. 
										//Par�ac�klar martisindeki elemanlara bakarak hangi t�r par�ac�k olduklar�n� bulur.
	int pSayisi=0, PSayisi=0, eSayisi=0, ESayisi=0, karsitMaddeSayisi, ESon, pSon;
	int i;                               
	for(i=0; i<parcacikSayisi; i++) {
		if(parcaciklar[i]=='p'){         //p-
			pSayisi++;
		}
		else if(parcaciklar[i]=='P') {   //p+
			PSayisi++;
		}
		else if(parcaciklar[i]=='e') {   //e-
			eSayisi++;
		}
		else if(parcaciklar[i]=='E') {   //e+
			ESayisi++;
		}
	}
	if(eSayisi>=ESayisi || PSayisi>=pSayisi){   //Z�t i�aretli atomlar birbirini yok eder. 
												//Bu y�zden e- say�s� e+ say�s�ndan b�y�k veya e�itse kar��t hidrolen olu�turulamaz ��nk� e+ say�s� 0 olur.
												//Ayn� durum p- i�in de ge�erlidir.
		karsitMaddeSayisi=0;
	}
	else {
		ESon=ESayisi-eSayisi;                   //Z�t i�aretli atomlar birbirini yok ettikten sonra kalan e+ ve p- say�s� bulunur.
		pSon=pSayisi-PSayisi;
		if(ESon>pSon){                          //Hangi par�ac�k daha az bulunuyorsa onun say�s� kadar kar��t madde olu�ur.
			karsitMaddeSayisi=pSon;
		}
		else{
			karsitMaddeSayisi=ESon;
		}
	}
	return karsitMaddeSayisi;                   //Olu�an kar��t madde say�s�n� d�nd�r�r.
}

void hamleYap(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi){

	if(*xSon<0 || *xSon>=N || *ySon<0 || *ySon>=M) {      //Matris d���na yap�lan hamlede kullan�c�n�n yeri de�i�mez.
		*xSon=*xIlk;
		*ySon=*yIlk;
		labirent[*xSon][*ySon]='X';
	}
	else if(labirent[*xSon][*ySon]=='0' || labirent[*xSon][*ySon]=='G'){    //0 veya G'ye yap�lan hamlede bu elemanlar yol gibi davrand��� i�in kullan�c�n�n yeri yapt��� tercihe g�re g�ncellenir.
		if(*hamleSayisi==1){                                                //�lk hamlede kullan�c�n�n geride b�rakt��� alana G yazd�r�l�r.
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		labirent[*xSon][*ySon]='X';
		*xIlk=*xSon;
		*yIlk=*ySon;
	}
	else if(labirent[*xSon][*ySon]=='1'){                  //1'ler duvar gibi davrand��� i�in kullan�c�n�n yeri de�i�mez.
		*xSon=*xIlk;
		*ySon=*yIlk;
		labirent[*xSon][*ySon]='X';
	}
	else if(labirent[*xSon][*ySon]=='P' || labirent[*xSon][*ySon]=='p' || labirent[*xSon][*ySon]=='E' || labirent[*xSon][*ySon]=='e') {
		parcaciklar[parcacikSayisi]=labirent[*xSon][*ySon];  //Hamle yap�lan par�ac�k parcaciklar dizisine eklenir.
		parcacikSayisi++;
		                                                     
		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		labirent[*xSon][*ySon]='X';                        //Kullan�c� atomalt� par�ac�klara hamle yaparsa tercihine g�re yeri g�ncellenir.
		*xIlk=*xSon;
		*yIlk=*ySon;
	}
	else if(labirent[*xSon][*ySon]=='C') {                 //Kullan�c� C'ye hamle yaparsa oyun sonlan�r.
		labirent[*xSon][*ySon]='X';

		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		*cikisMi=1;                                       //Oyun biti�i C'ye(��k��a) ula�arak olduysa cikisMi de�i�keni 1'e e�itlenir.
		
	}
	else if(labirent[*xSon][*ySon]=='K'){                 //Kullan�c� K'ye hamle yaparsa oyun sonlan�r.
		labirent[*xSon][*ySon]='X';

		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		*cikisMi=2;                                       //Oyun biti�i K'ye(karadeli�e) ula�arak olduysa cikisMi de�i�keni 2'ye e�itlenir.
	}	
}


void otomatikYon(int *yon){    //Otomatik modda kullan�c�dan y�n bilgisi almadan oyunu oynatmak i�in random say� �retilip y�n tu�lar�na atan�r.
	int randomYon;
	srand(time(NULL));
	randomYon= rand()%4;
	if(randomYon==0){
		*yon=72;     //yukar�
	}
	else if(randomYon==1){
		*yon=80;     //a�a��
	}
	else if(randomYon==2){
		*yon=75;     //sol
	}
	else if(randomYon==3){
		*yon=77;     //sa�
	}
	
}

void hamleAl(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi, int oyunModu){
	int yon;
	int i, karsitMaddeSayisi;
	int sureBasi, sureSonu, sure=0;
	printLabirent(labirent,N,M);
	
	sureBasi=time(NULL);             //30 saniye sonra oyunu bitirebilmek i�in ba�lang�� zaman�n� al�r.
	do {
		if(oyunModu==1){
			printf("Yon girin\n ");  //Manuel oyun modunda kullan�c�dan y�n girmesi beklenir.
        	yon=getch(); 
		}
		else if(oyunModu==2){
			otomatikYon(&yon);       //Otomatik modda oynanmak istendi�inde otomatikYon fonkdiyonunu �a��r�r.
		}

        switch(yon) {
        	(*hamleSayisi)++;
			printLabirent(labirent,N,M);
            case 72:                 //Yukar� durumunda X'in yeni konumu belirlenir ve hamleYap fonksiyonu �a�r�l�r.
                *xSon=*xIlk-1;
                *ySon=*yIlk;
				hamleYap(labirent, N, M, xIlk, yIlk, xSon, ySon, hamleSayisi, cikisMi);                
				if(oyunModu==2){     //Otomatik modda kullan�c�n�n takibini kolayla�t�rmak i�in sleep fonksiyonu �a�r�larak ekran temizleme(system("cls")) �ncesinde inceleme s�resi verilir.
					sleep(2);
				}
				system("cls");
                printLabirent(labirent,N,M);
                printParcacik();
				sureSonu=time(NULL); //30 saniye sonra oyunu bitirebilmek i�in biti� zaman�n� al�r.
				printf("\n");
                break;
            case 80:                 //A�a��
                *xSon=*xIlk+1;
                *ySon=*yIlk;
                hamleYap(labirent, N, M, xIlk, yIlk, xSon, ySon, hamleSayisi, cikisMi); 
                if(oyunModu==2){
					sleep(2);
				}
				system("cls");
                printLabirent(labirent,N,M);
                printParcacik();
				sureSonu=time(NULL);
				printf("\n");
                break;
            case 75:                  //Sol 
                *ySon=*yIlk-1;
                *xSon=*xIlk;
                hamleYap(labirent, N, M, xIlk, yIlk, xSon, ySon, hamleSayisi, cikisMi);
                if(oyunModu==2){
					sleep(2);
				}
				system("cls");
                printLabirent(labirent,N,M);
                printParcacik();
				sureSonu=time(NULL);
				printf("\n");
                break;
            case 77:                  //Sa� 
                *ySon=*yIlk+1;
                *xSon=*xIlk;
                hamleYap(labirent, N, M, xIlk, yIlk, xSon, ySon, hamleSayisi, cikisMi);
                if(oyunModu==2){
					sleep(2);
				}
				system("cls");
                printLabirent(labirent,N,M);
                printParcacik();
				sureSonu=time(NULL);
				printf("\n");
                break;
            default:
                break;
        }
        sure = difftime(sureSonu,sureBasi);  //Her hamle sonunda s�reyi kontrol eder ve 30 saniye a��lm��sa oyundan ��k�l�r.
        if(sure>30){                         //sureBasi d�ng�n�n d���nda oldu�u i�in her kontrolde o oyun i�inde ge�irilen toplam zaman bulunur.
        	*cikisMi=3;                      //Oyanama s�resinin 30 saniyeyi ge�mesi durumunda ��k�� yap�l�r ve cikisMi de�i�keni 3'e e�itlenir.
		}
    } while (yon!=27 && *cikisMi==0);        // ESC'ye bas�lma durumunda da oyun sonlan�r. cikisMi de�i�keniyle de karadelik(K), ��k��(C) ve s�re a��m� ile ��k�� durumlar� kontrol edilir.
	if(oyunModu==2){
		sleep(2);
	}
	system("cls");
	printLabirent(labirent,N,M);
	if(*cikisMi==1){                 //C'ye ula�ma durumu
		printf("Cikisa ulasildi!\n");
	}
	else if(*cikisMi==2){            //K'ye ula�ma durumu
		printf("Karadelige dustunuz!\n");
	}
	else if(*cikisMi==3){
		printf("Sureniz doldu!\n");  //Oyanama s�resinin 30 saniyeyi ge�me durumu
	}
	
	printParcacik();
	karsitMaddeSayisi=karsitHidrojenBul();                             //Kar��t hidrojen say�s� hesaplan�r ve yazd�r�l�r.
	printf("\n%d adet karsit madde uretildi.\n", karsitMaddeSayisi);
	*cikisMi=0;
}

void printParcacik(){                          //Toplanan atomalt� par�ac�klar� ekrana bast�r�r.
	int i;
	printf("Toplanan atomalti parcaciklar:\n");
	for(i=0; i<parcacikSayisi; i++){
        printf("%c ", parcaciklar[i]);
	}
}

int skorHesapla(int girisYapanKullanici){     //Kullan�c�n�n, kullan�c� listesindeki yerini parametre olarak al�r ve struct'ta o kullan�c�ya skoru atar.
	int skor;
	skor=karsitHidrojenBul();                 //KarsitHidrojenBul fonksiyonunu �a��r�r ve skora e�itler.
	kullaniciListesi[girisYapanKullanici].skor+=skor;
	return skor;                              //Skoru d�nd�r�r.
}

void enYuksekSkorlar(){                            //E�er varsa en y�ksek skoru olan 5 kullan�c� ve skorlar�n� yazd�r�r.
	int i, j, maxIndex, maxSkor=-1;
	int skorlar[kullaniciSayisi], enYuksekSkorlarIndex[5];
	for(i=0; i<kullaniciSayisi; i++){
		skorlar[i]=kullaniciListesi[i].skor;       //Skorlar bir dizide tutulur.  
	}
	for(i=1; i<=5; i++){
		for(j=0; j<kullaniciSayisi; j++){
			if(skorlar[j]>=maxSkor){               //j'nin her art���nda skorlar dizisindeki en b�y�k eleman� maxSkor de�i�kenine atar.
				maxSkor=skorlar[j];
				maxIndex=j;                        //maxIndex de�i�keni ise skorlar dizisindeki en b�y�k eleman�n indeksini tutar.
			}
		}
		enYuksekSkorlarIndex[i]=maxIndex;                  //enYuksekSkorlarIndex dizisi en b�y�k eleman�n skorlar dizisindeki indeksini tutar. 
		                                                   //Skorlar dizisindeki index ayn� zamanda kullan�c� listesindeki indekstir ��nk� skorlar s�ral� �ekilde diziye aktar�lm���tr.
		skorlar[maxIndex]=-1;                              //enYuksekSkorlarIndex dizisine eklendikten sonra o anki en b�y�k eleman diziden ��kart�l�r ve sonraki d�ng�de 2. en b�y�k eleman i�in i�lem tekrarlan�r.
		maxSkor=-1;                                        
		maxIndex=-1;	
	}
	printf("En Yuksek Skorlar:\n");
	if((kullaniciSayisi+1)<5){             //E�er 5'ten az kullan�c� varsa var olan kullan�c�lar�n skorlar�n� b�y�kten k����e do�ru olacak �ekilde yazd�r�r.
		for(i=1; i<=kullaniciSayisi; i++){
			printf("%d. Kullanici adi: %s --> Skor: %d\n",i, kullaniciListesi[enYuksekSkorlarIndex[i]].kullaniciAdi, kullaniciListesi[enYuksekSkorlarIndex[i]].skor );
		}
	}
	else{
		for(i=1; i<=5; i++){              //Kullan�c� say�s� 5 veya daha fazlaysa en y�ksek 5 skoru yazd�r�r.
			printf("%d. Kullanici adi: %s --> Skor: %d\n",i, kullaniciListesi[enYuksekSkorlarIndex[i]].kullaniciAdi, kullaniciListesi[enYuksekSkorlarIndex[i]].skor );
		}
	}	
}

int main(){
	int N,M;                                 //Matris boyutlar�
	struct Kullanici kullanici;              //Structure'da tutulmak �zere olu�turulan kullan�c�
	char** labirent;
	char dosyaAdi[MAX];
	int i,j;
	int hesabaGirisSecim;
	int oyunIciSecim;
	int girisYapanKullanici;
	int xIlk, yIlk, xSon, ySon;               //Kullan�c�n�n bulundu�u konumun hamle �ncesi ve hamle sonras� durumlar�n� tutarlar.
	int hamleSayisi=0;
	int cikisMi=0;                            //��k�� durumlar�n� kontrol eder.
	int oyunModu;                             //Otomatik veya manuel mod
	
	hesabaGirisSecim=hesabaGirisMenusu();     //hesabaGirisMenusu �a�r�l�r ve hesap olu�turma ya da login se�imi yap�l�r.
	do{
		if(hesabaGirisSecim==1){
			girisYapanKullanici=hesapOlustur(&kullanici);
			binaryKayit();                    //Her yeni kullan�c�n�n bilgileri binary olarak saklan�r.
		}
		else if(hesabaGirisSecim==2){
			girisYapanKullanici=login(&kullanici);
			while(girisYapanKullanici==-1){   //Yanl�� kullan�c� ad� veya �ifre durumu
				girisYapanKullanici=login(&kullanici);
			}
		}
		oyunIciSecim=oyunIciMenu();           //oyunIciMenu �a�r�l�r ve en y�ksek skorlar, oyun kurallar�, oyunu ba�latma, kullan�c� de�i�tirme aras�ndan se�im al�n�r.
		do{
			if(oyunIciSecim==1){     
				enYuksekSkorlar();
			}
			else if(oyunIciSecim==2){
				printOyunKurallari();
			}
			else if(oyunIciSecim==3){
				printf("Manuel modda oynamak icin 1\nOtomatik oynatma icin 2 girin\nSecim: ");
				scanf("%d", &oyunModu);
				printf("Hazir dosyalar:\nlabirent1(6x10).txt\tlabirent2(5x8).txt\tlabirent3(10x10).txt\n"
					"Dosya isimlerindeki parantez icindeki kisimlar labirentin satir ve sutun sayisini belirtir.\n"
					"Ornek: labirent1(5x8).txt -> 5 satir, 8 sutun icerir.\n");
				printf("Hazir dosyalardan birinin veya kendi dosyanizin adini giriniz: ");
				scanf("%s", dosyaAdi);
				printf("Labirentin satir sayisini giriniz: ");
				scanf("%d", &N);
				printf("Labirentin sutun sayisini giriniz: ");
				scanf("%d", &M);
				labirent = labirentYerAc(N,M);                    //labirentYerAc �a�r�larak labirente dinamik olarak yer a�ar
				readFromFile(labirent, dosyaAdi,N,M);             //readFromFile �a�r�larak girilen isimdeki dosya okunur.
				baslangicKonumuBul(labirent,N,M,&xIlk,&yIlk);     //baslangicKonumuBul �a�rlarak ba�lang��taki X konumu bulunur.
				hamleAl(labirent, N, M, &xIlk, &yIlk, &xSon, &ySon, &hamleSayisi, &cikisMi, oyunModu);   //hamleAl �a�r�larak kullan�c�dan hamle al�n�r. 
																										 //hamleAl fonksiyonunun i�inde halmeYap fonksiyonu �a�r�l�r ve hamle ger�ekle�tirilir.
				skorHesapla(girisYapanKullanici);                 //Oyun sonland���nda skorHesapla �a�r�larak skor hesaplan�r.
				hamleSayisi=0;                                    //Bir sonraki oyunda tekrar kullan�lmak i�in hamleSayisi ve parcacikSayisi s�f�rlan�r.
				parcacikSayisi=0;
				freeLabirent(labirent,N);                         //Oyun sonland���nda Labirent i�in ayr�lan yer serbest b�rak�l�r.
			}	
			oyunIciSecim=oyunIciMenu();
		}while(oyunIciSecim!=4);                                  //oyunIciMenu'de 4 (yani kullan�c� de�i�tirme ya da ��k��) se�ilmedi�i s�rece oyunIciSecim al�nmaya devam edilir.  
		hesabaGirisSecim=hesabaGirisMenusu();
		if(hesabaGirisSecim==3){                                  
			printf("Cikis yapiliyor.");
		}
	}while(hesabaGirisSecim!=3);                                  //hesabaGirisMenusu'nde 3 (yani ��k��) se�ilmedi�i s�rece hesabaGirisSecim al�nmaya devam edilir.
	
}
