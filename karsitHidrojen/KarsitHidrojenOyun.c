#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#define MAX 20
struct Kullanici {        //Kullanýcý bilgileri structure'da tutuluyor
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

int hesabaGirisMenusu() {     //Kullanýcýnýn oyunu açtýðýnda gördüðü ilk menü.
	int hesabaGirisSecim;     //Hesap oluþturma, login veya çýkýþ iþlemlerini seçtirir.
	printf("Karsit Madde Labirenti'ne hosgeldiniz!\nHesap olusturmak icin 1\nLogin icin 2\nCikis yapmak icin 3 giriniz.\nSecim: ");
	scanf("%d", &hesabaGirisSecim);
	return hesabaGirisSecim;
}

int oyunIciMenu() {           //Kullanýcýnýn hesap oluþturdukttan veya login olduktan sonra gördüðü menü.
	int oyunIciSecim;         //En yüksek 5 skoru görüntüleme, oyun kurallarýný görüntüleme, oyuna baþlama veya kullanýcý deðiþtirme iþlemlerini seçtirir.
	printf("\nEn yuksek skorlar ve sahiplerini goruntulemek icin 1\nOyun kurallari icin 2\nOyuna baslamak icin 3\n"
		"Kullanici degistirmek veya cikis yapmak icin 4 giriniz.\nSecim: ");
	scanf("%d", &oyunIciSecim);
	return oyunIciSecim;
}

void printOyunKurallari() {   //Oyun kurallarýný yazdýrýr.
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

int hesapOlustur(struct Kullanici *kullanici){                 //Hesap oluþturur ve struct'a kaydeder.
	printf("\nHesap Olusturma\n");
	printf("Isim: ");
	scanf("%s", kullanici->isim);
	printf("Soyisim: ");
	scanf("%s", kullanici->soyisim);
	do{
		printf("Kullanici Adi: ");
		scanf("%s", kullanici->kullaniciAdi);
		if(kullaniciAdiKontrol(kullanici->kullaniciAdi)){     //Kullanýcý adýnýn daha önceden alýnýp alýnmadýðýný kontrol eden kullaniciAdiKontrol fonksiyonunu çaðýrýr.
			printf("Kullanici adi önceden alinmis. Yeni bir kullanici adi girin.\n");
		}
	}while(kullaniciAdiKontrol(kullanici->kullaniciAdi));
	printf("Sifre: ");
	scanf("%s", kullanici->sifre);
	kullanici->skor=0;
	kullaniciListesi[kullaniciSayisi]= *kullanici;
	kullaniciSayisi++;
	return kullaniciSayisi-1;                                 //Fonksiyon parametre olarak aldýðý kullanýcýnýn, kullanýcýlarýn tutlduðu listede kaçýncý sýrada olduðunu döndürür. 
}

int kullaniciAdiKontrol(char *kullaniciAdi){                  //Kullanýcý adýnýn daha önce alýnýp alýnmadýðýný kontrol eder.
	int i;
	for(i=0; i<kullaniciSayisi; i++){
		if(strcmp(kullaniciAdi, kullaniciListesi[i].kullaniciAdi)==0){
			return 1;                                        //Eðer kullanýcý adý önceden alýnmýþsa 1 döndürür. Alýnmamýþsa o döndürür.
		}
	}
	return 0;
}

int login(struct Kullanici *kullanici){                     //Login iþlemini gerçekleþtirir.
	char loginAd[50], loginSifre[50];
	int i, kullaniciKontrol;
	
	printf("Kullanici adi: ");
	scanf("%s", loginAd);
	printf("Sifre: ");
	scanf("%s", loginSifre);
	for(i=0; i<kullaniciSayisi; i++){                      //Tüm kullanýcýlar gezilerek login olmak istenen kullanýcý aranýr.
		kullaniciKontrol=0;
		if(strcmp(kullaniciListesi[i].kullaniciAdi, loginAd) == 0){
			kullaniciKontrol++;
		}
		if(strcmp(kullaniciListesi[i].sifre, loginSifre) == 0){
			kullaniciKontrol++;
		}
		if(kullaniciKontrol==2){
			printf("Login basarili!\nHosgeldin %s", kullaniciListesi[i].kullaniciAdi);    //Kullanýcý adý ve þifre doðruysa login gerçekleþtirilmiþ olur ve kullanýcýnýn kullanýcý listesindeki sýrasýný döndürür.
			return i;
		}
	}
	printf("\nKullanici bulunamadi. Yeniden deneyin.\n");                     //Kullanýcý adý veya þifre yanlýþsa uyarý verir. 
	return -1;
		
}

void binaryKayit(){                              //Hesap oluþturulduðunda bilgiler binary olarak kullaniciData.txt dosyasýna kaydedilir. 
	struct Kullanici kullanici;
	FILE *bfptr = fopen("kullaniciData.txt", "wb");
	if (bfptr == NULL) {
        printf("Binary veri tutulacak dosya acilamadi.\n");
        exit(0);
    }
    fwrite(&kullanici, sizeof(struct Kullanici), 1, bfptr);
    fclose(bfptr);
}
 
void readFromFile(char** labirent, char *fileName, int N, int M) {    //Kullanýcýdan adý alýnan dosyadaki matris bilgisi okunur
                                                                      //ve dinamik hafýza ile yer açýlan labirent matrisine okunan bilgiler yazýlýr.
	int i, j;
	int elemanSayisi;
    FILE* fptr = fopen(fileName, "r");                                //Dosya okuma modunda açýlýr.

    if (fptr==NULL) {
        printf("Dosya acilamadi.\n");
        exit(1);
    }
    
    for (i=0; i<N; i++) {
        for (j=0; j<M; j++) {
            if (fscanf(fptr, "%c", &labirent[i][j]) != 1) {           //Dosyadan okunup labirente yazýlýr.
                printf("Dosya okuma hatasi.\n");
                exit(1);
            }
        }
    }
    fclose(fptr);
}

char** labirentYerAc(int N, int M){                 //Kullanýcýdan alýnan matris boyutlarýna göre labirent için dimanik olarak hafýzada yer açýlýr.
	int i;
	char** labirent=(char**)malloc(N * sizeof(char*));
	for(i=0; i<N; i++) {
        labirent[i] = (char*)malloc(M * sizeof(char));
    }
    return labirent;
}

void freeLabirent(char** labirent, int N) {         //Oyun sonlandýðýnda matris için ayrýlan yeri serbest býrakýr.
    int i;
    for(i = 0; i < N; i++) {
        free(labirent[i]);
    }
    free(labirent);
}


void printLabirent(char** labirent, int N, int M) {    //Labirent matrisini ekrana yazdýrýr.
	int i,j;
    for(i=0; i<N; i++) {
        for(j=0; j<M; j++) {
            printf("%c\t", labirent[i][j]);
        }
    	printf("\n");
    }
}

void baslangicKonumuBul(char** labirent, int N, int M, int *x, int *y){   //Oyunu baþlatmak için labirent içinde gezip X'i yani baþlangýç konumunu bulur.
																		  //Bulduðu koordinatlarý x ve y olarak tutar.
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

int karsitHidrojenBul() {               //e+(E) ve p-(p) birleþimi karþýt hidrojen oluþturur. 
										//Parçacýklar martisindeki elemanlara bakarak hangi tür parçacýk olduklarýný bulur.
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
	if(eSayisi>=ESayisi || PSayisi>=pSayisi){   //Zýt iþaretli atomlar birbirini yok eder. 
												//Bu yüzden e- sayýsý e+ sayýsýndan büyük veya eþitse karþýt hidrolen oluþturulamaz çünkü e+ sayýsý 0 olur.
												//Ayný durum p- için de geçerlidir.
		karsitMaddeSayisi=0;
	}
	else {
		ESon=ESayisi-eSayisi;                   //Zýt iþaretli atomlar birbirini yok ettikten sonra kalan e+ ve p- sayýsý bulunur.
		pSon=pSayisi-PSayisi;
		if(ESon>pSon){                          //Hangi parçacýk daha az bulunuyorsa onun sayýsý kadar karþýt madde oluþur.
			karsitMaddeSayisi=pSon;
		}
		else{
			karsitMaddeSayisi=ESon;
		}
	}
	return karsitMaddeSayisi;                   //Oluþan karþýt madde sayýsýný döndürür.
}

void hamleYap(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi){

	if(*xSon<0 || *xSon>=N || *ySon<0 || *ySon>=M) {      //Matris dýþýna yapýlan hamlede kullanýcýnýn yeri deðiþmez.
		*xSon=*xIlk;
		*ySon=*yIlk;
		labirent[*xSon][*ySon]='X';
	}
	else if(labirent[*xSon][*ySon]=='0' || labirent[*xSon][*ySon]=='G'){    //0 veya G'ye yapýlan hamlede bu elemanlar yol gibi davrandýðý için kullanýcýnýn yeri yaptýðý tercihe göre güncellenir.
		if(*hamleSayisi==1){                                                //Ýlk hamlede kullanýcýnýn geride býraktýðý alana G yazdýrýlýr.
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		labirent[*xSon][*ySon]='X';
		*xIlk=*xSon;
		*yIlk=*ySon;
	}
	else if(labirent[*xSon][*ySon]=='1'){                  //1'ler duvar gibi davrandýðý için kullanýcýnýn yeri deðiþmez.
		*xSon=*xIlk;
		*ySon=*yIlk;
		labirent[*xSon][*ySon]='X';
	}
	else if(labirent[*xSon][*ySon]=='P' || labirent[*xSon][*ySon]=='p' || labirent[*xSon][*ySon]=='E' || labirent[*xSon][*ySon]=='e') {
		parcaciklar[parcacikSayisi]=labirent[*xSon][*ySon];  //Hamle yapýlan parçacýk parcaciklar dizisine eklenir.
		parcacikSayisi++;
		                                                     
		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		labirent[*xSon][*ySon]='X';                        //Kullanýcý atomaltý parçacýklara hamle yaparsa tercihine göre yeri güncellenir.
		*xIlk=*xSon;
		*yIlk=*ySon;
	}
	else if(labirent[*xSon][*ySon]=='C') {                 //Kullanýcý C'ye hamle yaparsa oyun sonlanýr.
		labirent[*xSon][*ySon]='X';

		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		*cikisMi=1;                                       //Oyun bitiþi C'ye(çýkýþa) ulaþarak olduysa cikisMi deðiþkeni 1'e eþitlenir.
		
	}
	else if(labirent[*xSon][*ySon]=='K'){                 //Kullanýcý K'ye hamle yaparsa oyun sonlanýr.
		labirent[*xSon][*ySon]='X';

		if(*hamleSayisi==1){
			labirent[*xIlk][*yIlk]='G';
		}else{
			labirent[*xIlk][*yIlk]='0';
		}
		*cikisMi=2;                                       //Oyun bitiþi K'ye(karadeliðe) ulaþarak olduysa cikisMi deðiþkeni 2'ye eþitlenir.
	}	
}


void otomatikYon(int *yon){    //Otomatik modda kullanýcýdan yön bilgisi almadan oyunu oynatmak için random sayý üretilip yön tuþlarýna atanýr.
	int randomYon;
	srand(time(NULL));
	randomYon= rand()%4;
	if(randomYon==0){
		*yon=72;     //yukarý
	}
	else if(randomYon==1){
		*yon=80;     //aþaðý
	}
	else if(randomYon==2){
		*yon=75;     //sol
	}
	else if(randomYon==3){
		*yon=77;     //sað
	}
	
}

void hamleAl(char** labirent, int N, int M, int *xIlk, int *yIlk, int *xSon, int *ySon, int *hamleSayisi, int *cikisMi, int oyunModu){
	int yon;
	int i, karsitMaddeSayisi;
	int sureBasi, sureSonu, sure=0;
	printLabirent(labirent,N,M);
	
	sureBasi=time(NULL);             //30 saniye sonra oyunu bitirebilmek için baþlangýç zamanýný alýr.
	do {
		if(oyunModu==1){
			printf("Yon girin\n ");  //Manuel oyun modunda kullanýcýdan yön girmesi beklenir.
        	yon=getch(); 
		}
		else if(oyunModu==2){
			otomatikYon(&yon);       //Otomatik modda oynanmak istendiðinde otomatikYon fonkdiyonunu çaðýrýr.
		}

        switch(yon) {
        	(*hamleSayisi)++;
			printLabirent(labirent,N,M);
            case 72:                 //Yukarý durumunda X'in yeni konumu belirlenir ve hamleYap fonksiyonu çaðrýlýr.
                *xSon=*xIlk-1;
                *ySon=*yIlk;
				hamleYap(labirent, N, M, xIlk, yIlk, xSon, ySon, hamleSayisi, cikisMi);                
				if(oyunModu==2){     //Otomatik modda kullanýcýnýn takibini kolaylaþtýrmak için sleep fonksiyonu çaðrýlarak ekran temizleme(system("cls")) öncesinde inceleme süresi verilir.
					sleep(2);
				}
				system("cls");
                printLabirent(labirent,N,M);
                printParcacik();
				sureSonu=time(NULL); //30 saniye sonra oyunu bitirebilmek için bitiþ zamanýný alýr.
				printf("\n");
                break;
            case 80:                 //Aþaðý
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
            case 77:                  //Sað 
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
        sure = difftime(sureSonu,sureBasi);  //Her hamle sonunda süreyi kontrol eder ve 30 saniye aþýlmýþsa oyundan çýkýlýr.
        if(sure>30){                         //sureBasi döngünün dýþýnda olduðu için her kontrolde o oyun içinde geçirilen toplam zaman bulunur.
        	*cikisMi=3;                      //Oyanama süresinin 30 saniyeyi geçmesi durumunda çýkýþ yapýlýr ve cikisMi deðiþkeni 3'e eþitlenir.
		}
    } while (yon!=27 && *cikisMi==0);        // ESC'ye basýlma durumunda da oyun sonlanýr. cikisMi deðiþkeniyle de karadelik(K), çýkýþ(C) ve süre aþýmý ile çýkýþ durumlarý kontrol edilir.
	if(oyunModu==2){
		sleep(2);
	}
	system("cls");
	printLabirent(labirent,N,M);
	if(*cikisMi==1){                 //C'ye ulaþma durumu
		printf("Cikisa ulasildi!\n");
	}
	else if(*cikisMi==2){            //K'ye ulaþma durumu
		printf("Karadelige dustunuz!\n");
	}
	else if(*cikisMi==3){
		printf("Sureniz doldu!\n");  //Oyanama süresinin 30 saniyeyi geçme durumu
	}
	
	printParcacik();
	karsitMaddeSayisi=karsitHidrojenBul();                             //Karþýt hidrojen sayýsý hesaplanýr ve yazdýrýlýr.
	printf("\n%d adet karsit madde uretildi.\n", karsitMaddeSayisi);
	*cikisMi=0;
}

void printParcacik(){                          //Toplanan atomaltý parçacýklarý ekrana bastýrýr.
	int i;
	printf("Toplanan atomalti parcaciklar:\n");
	for(i=0; i<parcacikSayisi; i++){
        printf("%c ", parcaciklar[i]);
	}
}

int skorHesapla(int girisYapanKullanici){     //Kullanýcýnýn, kullanýcý listesindeki yerini parametre olarak alýr ve struct'ta o kullanýcýya skoru atar.
	int skor;
	skor=karsitHidrojenBul();                 //KarsitHidrojenBul fonksiyonunu çaðýrýr ve skora eþitler.
	kullaniciListesi[girisYapanKullanici].skor+=skor;
	return skor;                              //Skoru döndürür.
}

void enYuksekSkorlar(){                            //Eðer varsa en yüksek skoru olan 5 kullanýcý ve skorlarýný yazdýrýr.
	int i, j, maxIndex, maxSkor=-1;
	int skorlar[kullaniciSayisi], enYuksekSkorlarIndex[5];
	for(i=0; i<kullaniciSayisi; i++){
		skorlar[i]=kullaniciListesi[i].skor;       //Skorlar bir dizide tutulur.  
	}
	for(i=1; i<=5; i++){
		for(j=0; j<kullaniciSayisi; j++){
			if(skorlar[j]>=maxSkor){               //j'nin her artýþýnda skorlar dizisindeki en büyük elemaný maxSkor deðiþkenine atar.
				maxSkor=skorlar[j];
				maxIndex=j;                        //maxIndex deðiþkeni ise skorlar dizisindeki en büyük elemanýn indeksini tutar.
			}
		}
		enYuksekSkorlarIndex[i]=maxIndex;                  //enYuksekSkorlarIndex dizisi en büyük elemanýn skorlar dizisindeki indeksini tutar. 
		                                                   //Skorlar dizisindeki index ayný zamanda kullanýcý listesindeki indekstir çünkü skorlar sýralý þekilde diziye aktarýlmýþýtr.
		skorlar[maxIndex]=-1;                              //enYuksekSkorlarIndex dizisine eklendikten sonra o anki en büyük eleman diziden çýkartýlýr ve sonraki döngüde 2. en büyük eleman için iþlem tekrarlanýr.
		maxSkor=-1;                                        
		maxIndex=-1;	
	}
	printf("En Yuksek Skorlar:\n");
	if((kullaniciSayisi+1)<5){             //Eðer 5'ten az kullanýcý varsa var olan kullanýcýlarýn skorlarýný büyükten küçüðe doðru olacak þekilde yazdýrýr.
		for(i=1; i<=kullaniciSayisi; i++){
			printf("%d. Kullanici adi: %s --> Skor: %d\n",i, kullaniciListesi[enYuksekSkorlarIndex[i]].kullaniciAdi, kullaniciListesi[enYuksekSkorlarIndex[i]].skor );
		}
	}
	else{
		for(i=1; i<=5; i++){              //Kullanýcý sayýsý 5 veya daha fazlaysa en yüksek 5 skoru yazdýrýr.
			printf("%d. Kullanici adi: %s --> Skor: %d\n",i, kullaniciListesi[enYuksekSkorlarIndex[i]].kullaniciAdi, kullaniciListesi[enYuksekSkorlarIndex[i]].skor );
		}
	}	
}

int main(){
	int N,M;                                 //Matris boyutlarý
	struct Kullanici kullanici;              //Structure'da tutulmak üzere oluþturulan kullanýcý
	char** labirent;
	char dosyaAdi[MAX];
	int i,j;
	int hesabaGirisSecim;
	int oyunIciSecim;
	int girisYapanKullanici;
	int xIlk, yIlk, xSon, ySon;               //Kullanýcýnýn bulunduðu konumun hamle öncesi ve hamle sonrasý durumlarýný tutarlar.
	int hamleSayisi=0;
	int cikisMi=0;                            //Çýkýþ durumlarýný kontrol eder.
	int oyunModu;                             //Otomatik veya manuel mod
	
	hesabaGirisSecim=hesabaGirisMenusu();     //hesabaGirisMenusu çaðrýlýr ve hesap oluþturma ya da login seçimi yapýlýr.
	do{
		if(hesabaGirisSecim==1){
			girisYapanKullanici=hesapOlustur(&kullanici);
			binaryKayit();                    //Her yeni kullanýcýnýn bilgileri binary olarak saklanýr.
		}
		else if(hesabaGirisSecim==2){
			girisYapanKullanici=login(&kullanici);
			while(girisYapanKullanici==-1){   //Yanlýþ kullanýcý adý veya þifre durumu
				girisYapanKullanici=login(&kullanici);
			}
		}
		oyunIciSecim=oyunIciMenu();           //oyunIciMenu çaðrýlýr ve en yüksek skorlar, oyun kurallarý, oyunu baþlatma, kullanýcý deðiþtirme arasýndan seçim alýnýr.
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
				labirent = labirentYerAc(N,M);                    //labirentYerAc çaðrýlarak labirente dinamik olarak yer açar
				readFromFile(labirent, dosyaAdi,N,M);             //readFromFile çaðrýlarak girilen isimdeki dosya okunur.
				baslangicKonumuBul(labirent,N,M,&xIlk,&yIlk);     //baslangicKonumuBul çaðrlarak baþlangýçtaki X konumu bulunur.
				hamleAl(labirent, N, M, &xIlk, &yIlk, &xSon, &ySon, &hamleSayisi, &cikisMi, oyunModu);   //hamleAl çaðrýlarak kullanýcýdan hamle alýnýr. 
																										 //hamleAl fonksiyonunun içinde halmeYap fonksiyonu çaðrýlýr ve hamle gerçekleþtirilir.
				skorHesapla(girisYapanKullanici);                 //Oyun sonlandýðýnda skorHesapla çaðrýlarak skor hesaplanýr.
				hamleSayisi=0;                                    //Bir sonraki oyunda tekrar kullanýlmak için hamleSayisi ve parcacikSayisi sýfýrlanýr.
				parcacikSayisi=0;
				freeLabirent(labirent,N);                         //Oyun sonlandýðýnda Labirent için ayrýlan yer serbest býrakýlýr.
			}	
			oyunIciSecim=oyunIciMenu();
		}while(oyunIciSecim!=4);                                  //oyunIciMenu'de 4 (yani kullanýcý deðiþtirme ya da çýkýþ) seçilmediði sürece oyunIciSecim alýnmaya devam edilir.  
		hesabaGirisSecim=hesabaGirisMenusu();
		if(hesabaGirisSecim==3){                                  
			printf("Cikis yapiliyor.");
		}
	}while(hesabaGirisSecim!=3);                                  //hesabaGirisMenusu'nde 3 (yani çýkýþ) seçilmediði sürece hesabaGirisSecim alýnmaya devam edilir.
	
}
