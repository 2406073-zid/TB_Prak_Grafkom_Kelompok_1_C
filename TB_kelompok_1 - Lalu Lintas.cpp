#include <GL/freeglut.h> 
#include <math.h>        
#include <stdlib.h>      
#include <iostream>      

const float PI = 3.14159265f; // perhitungan dalam trigonometri


// variabel untuk kamera
float kameraPosX = 0.0f, kameraPosY = 300.0f, kameraPosZ = 600.0f; 
float kameraYaw = 0.0f, kameraPitch = 20.0f; 
float kec = 10.0f; 
bool modeKameraMobil = false; 

// variabel untuk tranformasi
float skalaTugu = 1.0f;        
float skalaPolisi = 1.0f;      
float rotasiTugu = 0.0f;       

// variabel untuk mobil
float pMobilX = 0.0f;          
float pMobilZ = 600.0f;        
float pMobilSudut = 180.0f;    
float pMobilKecepatan = 5.0f;  
float pMobilSpeedMaks = 5.0f;  

// variabel untuk pergerakan matahari
float waktuSiklus = 0.0f;      
float kecepatanWaktu = 0.5f;   
bool otomatisWaktu = true;    

// variabel untuk pencahayaan
bool lampuSpotNyala = true;    
float sudutLampuSpot = 15.0f;  
float jangkauanCahaya = 0.0011f; 

// variabel untuk inputan
bool tombol[256] = { false };          
bool tombolSpesial[256] = { false };   
bool mouseAktif = false;               
int mouseTerakhirX, mouseTerakhirY;    

// variabel untuk kota
const int UKURAN_KOTA = 6;              
const int UKURAN_BLOK = 120;            
const float UKURAN_GEDUNG = 120.0f;    
const float LEBAR_JALAN = 100.0f;      


// digunakan untuk membuat wadah cube yang tinggal digonta-ganti transformasinya
void gambarBagianModel(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b, bool neon = false) {
    glPushMatrix(); 
    glTranslatef(x, y + (sy / 2.0f), z); 
    glScalef(sx, sy, sz); 

    if (neon) {
        glDisable(GL_LIGHTING); 
        glColor3f(r * 1.5f, g * 1.5f, b * 1.5f); 
    } else {
        glColor3f(r, g, b); 
    }

    glutSolidCube(1.0f); 
    
    if (neon) glEnable(GL_LIGHTING); 
    glPopMatrix(); 
}

// dibuat oleh fauzan alamsyah
void gambarTugu() {
    GLUquadric* q = gluNewQuadric(); 
    glPushMatrix();
    glRotatef(rotasiTugu, 0, 1, 0); 
    glScalef(skalaTugu, skalaTugu, skalaTugu); 
    
    // bawahan tugu
    gambarBagianModel(0, 2.3f, 0, 60, 4, 60, 0.0f, 0.0f, 0.2f); 
    gambarBagianModel(0, 6.3f, 0, 45, 6, 45, 0.7f, 0.7f, 0.0f); 
    gambarBagianModel(0, 12.3f, 0, 30, 8, 30, 0.0f, 0.f, 0.2f); 

    // tiang tugu
    glPushMatrix();
    glTranslatef(0, 20.0f, 0); 
    glRotatef(-90, 1, 0, 0); 
    glColor3f(0.9f, 0.9f, 0.9f); 
    gluCylinder(q, 10.0f, 5.0f, 150.0f, 16, 1); 
    glPopMatrix();

    // puncak tugu
    glPushMatrix();
    glTranslatef(0, 175.0f, 0); 
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.5f); 
    glutSolidSphere(12.0f, 8, 8); 
    glColor3f(1.0f, 0.9f, 0.0f);
    glutWireSphere(15.0f, 10, 10); 
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
    glPopMatrix();
    gluDeleteQuadric(q); 
}

//  alun-alun tarkid
void gambarAlunAlun(float ukuran) {
    GLUquadric* q = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0, 0.1f, 0); 
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.0f, 0.2f, 0.0f); 
    gluDisk(q, 0, ukuran * 0.3f, 64, 1); 
    glPopMatrix();

    // jalanan memutar
    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.0f, 0.0f, 0.0f); 
    gluDisk(q, 0, ukuran * 0.2f, 64, 1);
    glTranslatef(0, 0, 0.1f);
    glColor3f(0.0f, 0.2f, 0.0f); 
    gluDisk(q, 0, ukuran * 0.1f, 64, 1);
    glPopMatrix();

    gambarTugu(); 

    // pohon yang memutari alun-alun
    for (int i = 0; i < 24; i++) {
        float sudut = i * (360.0f / 24.0f) * PI / 180.0f; 
        float jarak = ukuran * 0.25f; 
        float px = cos(sudut) * jarak; 
        float pz = sin(sudut) * jarak; 

        // batang pohon
        glPushMatrix();
        glTranslatef(px, 0.0f, pz);
        glRotatef(-90, 1, 0, 0);
        glColor3f(0.35f, 0.2f, 0.1f); 
        gluCylinder(q, 3.5f, 3.0f, 35.0f, 10, 1);
        glPopMatrix();

        // daun pohon
        for (int lapis = 0; lapis < 3; lapis++) {
            glPushMatrix();
            glTranslatef(px, 25.0f + (lapis * 12.0f), pz);
            glRotatef(-90, 1, 0, 0);
            glColor3f(0.1f + (lapis * 0.05f), 0.4f + (lapis * 0.1f), 0.1f); 
            glutSolidCone(18.0f - (lapis * 4.0f), 25.0f, 10, 2);
            glPopMatrix();
        }
    }
    gluDeleteQuadric(q);
}
// akhir dari kode yang dibuat oleh fauzan alamsyah

// lampu lalu lintas
// kode yang dibuat oleh andhika eka
void gambarLampuLaluLintas(float x, float z, float rotasi) {
    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(rotasi, 0, 1, 0); 

    GLUquadric* q = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); 
    glColor3f(0.1f, 0.1f, 0.1f); 
    gluCylinder(q, 2.0f, 2.0f, 60.0f, 20, 1); 
    glPopMatrix();

    glRotatef(45.0f, 0.0f, 1.0f, 0.0f); 

    gambarBagianModel(0, 55, 15, 4, 4, 30, 0.1f, 0.1f, 0.1f); 
    gambarBagianModel(0, 49, 30, 5, 16, 4, 0.1f, 0.1f, 0.1f); 

    // lampu beureum
    glPushMatrix();
    glTranslatef(-3, 62.0f, 30.0f); 
    glDisable(GL_LIGHTING); glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(2.0f, 16, 16); glEnable(GL_LIGHTING);
    glPopMatrix();

    // lampu koneng
    glPushMatrix();
    glTranslatef(-3, 57.0f, 30.0f);
    glDisable(GL_LIGHTING); glColor3f(1.0f, 0.7f, 0.0f);
    glutSolidSphere(2.0f, 16, 16); glEnable(GL_LIGHTING);
    glPopMatrix();

    // lampu hejo
    glPushMatrix();
    glTranslatef(-3, 52.0f, 30.0f);
    glDisable(GL_LIGHTING); glColor3f(0.0f, 1.0f, 0.0f);
    glutSolidSphere(2.0f, 16, 16); glEnable(GL_LIGHTING);
    glPopMatrix();

    gluDeleteQuadric(q);
    glPopMatrix();
}
// akhir dari kode yang dibuat oleh andhika eka

// gedung yang ditempatkan secara random dengan jumlah yang bisa ditentukan
void gambarGedungModel(float x, float z, int benih) {
    srand(benih); 
    if (rand() % 10 > 10) return; 

    float tinggi = 120.0f + (rand() % 280); 
    float lebar = 70.0f + (rand() % 30); 
    float r = (rand() % 100 + 50) / 255.0f; 
    float g = (rand() % 100 + 50) / 255.0f; 
    float b = (rand() % 100 + 50) / 255.0f; 

    gambarBagianModel(x, 0.5f, z, lebar, tinggi, lebar, r, g, b); 

    // untuk pintu masuk
    float tinggiPintu = 30.0f;
    float posZ_Pintu = z + (lebar / 2.0f) + 0.5f;
    gambarBagianModel(x, 0.5f, posZ_Pintu, (lebar * 0.4f) + 6, tinggiPintu + 3, 2, 0.1f, 0.1f, 0.1f); 
    
    // untuk tiang hiasan
    for (int i = -1; i <= 1; i += 2) {
        gambarBagianModel(x + (i * 3.0f), 12.0f, posZ_Pintu + 1.2f, 1.5f, 8.0f, 1.5f, 1.0f, 0.8f, 0.0f); 
    }
    
    // untuk jendela
    float ukKaca = 12.0f;
    float jarakLantai = 25.0f;
    float offsetKaca = lebar / 2.0f + 0.6f;

    for (float h = tinggiPintu + 15.0f; h < tinggi - 15.0f; h += jarakLantai) {
        for (int sisi = 0; sisi < 4; sisi++) { 
            for (float horiz = -1.0f; horiz <= 1.0f; horiz += 2.0f) { 
                float posH = horiz * (lebar * 0.25f);
                glPushMatrix();
                glTranslatef(x, h, z);
                glRotatef(sisi * 90.0f, 0, 1, 0);
                gambarBagianModel(posH, 0, offsetKaca + 0.5f, ukKaca, ukKaca, 1.0f, 0.4f, 0.9f, 1.0f, true); 
                glPopMatrix();
            }
        }
    }
    gambarBagianModel(x, tinggi, z, lebar + 8, 4, lebar + 8, 0.15f, 0.15f, 0.15f); 
}

//  mobil 
// kode yang dibuat oleh albar hifdzi
void gambarMobil(float x, float z, float rotasi, int benih) {
    srand(benih);
    float r = (rand() % 200 + 55) / 255.0f;
    float g = (rand() % 200 + 55) / 255.0f;
    float b = (rand() % 200 + 55) / 255.0f;

    glPushMatrix();
    glTranslatef(x, 3.0f, z);
    glRotatef(rotasi, 0, 1, 0);

    // untuk body mobil
    gambarBagianModel(0, 3.0f, 0, 22, 6, 46, r, g, b); // untuk body bawah
    gambarBagianModel(0, 9.0f, -4.0f, 20, 7, 22, r, g, b); // untuk body tengah
    gambarBagianModel(0, 9.5f, 7.1f, 19, 5, 0.5f, 0.5f, 1.0f, 1.0f, true); // untuk kaca depan
    gambarBagianModel(0, 9.5f, -15.1f, 19, 5, 0.5f, 0.5f, 1.0f, 1.0f, true); // untuk kaca belakang

    // untuk lampu mobil
    int posisiLampu[] = {-7, 7}; 
    for(int j = 0; j < 2; j++) {
        int i = posisiLampu[j];
        glPushMatrix();
        glTranslatef((float)i, 6.0f, 23.5f);
        glDisable(GL_LIGHTING); glColor3f(1.0f, 0.9f, 0.0f);
        glutSolidSphere(2.2f, 16, 16); glEnable(GL_LIGHTING);
        glPopMatrix();
    }

    gambarBagianModel(0.0f, 5.0f, 23.1f, 8, 2.5f, 0.5f, 0.2f, 0.2f, 0.2f); // untuk plat nomor

    // untuk roda
    float posRodaX[4] = { 11.5f, -11.5f, 11.5f, -11.5f };
    float posRodaZ[4] = { 15.0f, 15.0f, -15.0f, -15.0f };

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(posRodaX[i], 1.5f, posRodaZ[i]);
        glRotatef(posRodaX[i] > 0 ? 90 : -90, 0, 1, 0);

        GLUquadric* q = gluNewQuadric();
        glColor3f(0.05f, 0.05f, 0.05f); 
        gluCylinder(q, 5.0, 5.0, 5.5, 40, 1);

        glPushMatrix();
        glTranslatef(0, 0, 5.6f); 
        glColor3f(0.7f, 0.7f, 0.7f); 
        gluDisk(q, 3.5, 5.0, 40, 1);
        glDisable(GL_LIGHTING);
        glColor3f(0.2f, 0.2f, 0.2f); 
        for (int j = 0; j < 5; j++) { 
            glPushMatrix();
            glRotatef(j * 72, 0, 0, 1); 
            glTranslatef(1.8f, 0, 0);
            glScalef(3.5f, 0.8f, 0.2f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
        glEnable(GL_LIGHTING);
        glPopMatrix();
        gluDeleteQuadric(q);
        glPopMatrix();
    }
    glPopMatrix();
}
// akhir dari kode yang dbuat oleh albar hifdzi

// agar mobil tidak bisa menabrak bangunan lain atau agar tidak dapat melewati batas
bool cekTabrakan(float xBaru, float zBaru) {
    float langkah = UKURAN_GEDUNG + LEBAR_JALAN; 
    float totalPeta = UKURAN_KOTA * langkah;
    float batasPeta = totalPeta / 2.0f;
    float batasTepi = batasPeta - (-60.0f); 

    // untuk batas kota
    if (xBaru < -batasTepi || xBaru > batasTepi || zBaru < -batasTepi || zBaru > batasTepi) return true;

    // untuk alun-alun
    float jarakDariPusat = sqrt(xBaru * xBaru + zBaru * zBaru);
    if (jarakDariPusat < 200.0f) return true; 

    // untuk gedung
    int gridI = (int)((xBaru + batasPeta) / langkah);
    int gridJ = (int)((zBaru + batasPeta) / langkah);

    if (gridI >= 0 && gridI < UKURAN_KOTA && gridJ >= 0 && gridJ < UKURAN_KOTA) {
        bool areaAlunAlun = (gridI == 2 || gridI == 3) && (gridJ == 2 || gridJ == 3);
        if (!areaAlunAlun) {
            float pusatGedungX = -batasPeta + gridI * langkah + langkah / 2.0f;
            float pusatGedungZ = -batasPeta + gridJ * langkah + langkah / 2.0f;
            if (abs(xBaru - pusatGedungX) < 65.0f && abs(zBaru - pusatGedungZ) < 55.0f) return true; 
        }
    }
    return false;
}

// kantor polisi
// kode yang dibuat oleh m. zidane
void gambarKantorPolisi(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0, z);
    glScalef(skalaPolisi, skalaPolisi, skalaPolisi); 

    // struktur bangunan kantor polisi
    gambarBagianModel(0, 0, 0, 240, 5, 160, 0.2f, 0.2f, 0.2f); 
    gambarBagianModel(0, 5, 0, 220, 80, 140, 0.05f, 0.1f, 0.4f); 
    gambarBagianModel(0, 45, 0, 222, 10, 142, 1.0f, 1.0f, 1.0f); 

    // untuk jendela
    for(int i = -1; i <= 1; i += 2) {
        gambarBagianModel(i * 75, 10, 71.1f, 45, 40, 2, 0.1f, 0.1f, 0.1f);
        gambarBagianModel(i * 75, 22, 71.3f, 38, 35, 1, 0.4f, 0.9f, 1.0f, true);
    }

    // untuk pintu
    gambarBagianModel(0, 5, 75, 120, 85, 20, 0.1f, 0.15f, 0.5f);
    gambarBagianModel(0, 60, 90, 100, 5, 40, 0.3f, 0.3f, 0.3f);

    for(int i = -1; i <= 1; i += 2) {
        gambarBagianModel(i * 18, 5, 86.1f, 30, 48, 2, 0.05f, 0.05f, 0.05f);
        gambarBagianModel(i * 18, 8, 86.2f, 22, 42, 1, 0.8f, 1.0f, 1.0f, true);
        gambarBagianModel(i * 8, 20, 87.5f, 1.5f, 10, 1.5f, 1.0f, 0.8f, 0.0f);
    }

    // menggambar tulisan polisi di plang
    gambarBagianModel(0, 62, 86.0f, 106, 22, 3, 0.0f, 0.0f, 0.0f); 
    float lx = -42.0f; float ly = 66.0f; float lz = 88.0f; float s = 1.8f; float sp = 14.0f;
    glDisable(GL_LIGHTING); glColor3f(1.0f, 1.0f, 1.0f); 

    // huruf P
    gambarBagianModel(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly + 12, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly + 6, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx + 7, ly + 8, lz, s, 6, s, 1,1,1);
    lx += sp;
    // huruf O
    gambarBagianModel(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianModel(lx + 7, ly, lz, s, 14, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly + 12, lz, 7, s, s, 1,1,1);
    lx += sp;
    // huruf L
    gambarBagianModel(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);
    lx += sp;
    // huruf I
    gambarBagianModel(lx + 3.5f, ly, lz, s, 14, s, 1,1,1);

    // huruf S
    lx += sp; 
    gambarBagianModel(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly + 7, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx + 3.5f, ly + 13, lz, 7, s, s, 1,1,1);
    gambarBagianModel(lx, ly + 8, lz, s, 6, s, 1,1,1);
    gambarBagianModel(lx + 7, ly + 2, lz, s, 6, s, 1,1,1);
    
    // huruf I
    lx += sp; 
    gambarBagianModel(lx + 3.5f, ly, lz, s, 14, s, 1,1,1);

    glEnable(GL_LIGHTING);

    // untuk bagian sirine warna biru merah diatas kantor polisi
    int posisiSirine[] = {-20, 20};
    for(int j = 0; j < 2; j++) {
        int i = posisiSirine[j];
        glPushMatrix();
        glTranslatef((float)i, 94, 80);
        glRotatef(90, 1, 0, 0);
        GLUquadric *sir = gluNewQuadric();
        glColor3f(0.3f, 0.3f, 0.3f);
        gluCylinder(sir, 2.5f, 2.5f, 8.0f, 16, 1);
        gluDeleteQuadric(sir);
        glTranslatef(0, 0, -4.0f);
        if(i < 0) glColor3f(1,0,0); else glColor3f(0,0,1); 
        glutSolidSphere(4.0f, 20, 20);
        glPopMatrix();
    }

    // untuk menara kantor polisi
    glPushMatrix();
    glTranslatef(75, 0, -30);
    gambarBagianModel(0, 2.5f, 0, 45, 5, 45, 0.15f, 0.15f, 0.2f);
    glPushMatrix();
    glTranslatef(0, 85.0f, 0); glRotatef(-90, 1, 0, 0);
    GLUquadric *menara = gluNewQuadric();
    glColor3f(0.15f, 0.15f, 0.2f); gluCylinder(menara, 10.0f, 8.0f, 160.0f, 16, 1);
    gluDeleteQuadric(menara);
    glPopMatrix();
    gambarBagianModel(0, 200, 0, 30, 25, 30, 0.4f, 0.9f, 1.0f); 
    glPushMatrix();
    glTranslatef(0, 225, 0); glColor3f(0.2f, 0.5f, 0.8f); glutSolidSphere(12, 20, 20); 
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}
// akhir dari kode yanng dibuat oleh m. zidane

// kota
void gambarKotaModel() {
    float langkah = UKURAN_GEDUNG + LEBAR_JALAN;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;

    gambarBagianModel(0, -2.0f, 0, 1500, 2, 1500, 0.1f, 0.1f, 0.1f);  

    float ukAlunAlun = langkah * 3.0f;

    for (int i = 0; i < UKURAN_KOTA; i++) {
        for (int j = 0; j < UKURAN_KOTA; j++) {
            float x = -posisiAwal + i * langkah + langkah / 2.0f;
            float z = -posisiAwal + j * langkah + langkah / 2.0f;

            // untuk area tengah alun-alun
            if (abs(x) < ukAlunAlun / 2.0f && abs(z) < ukAlunAlun / 2.0f) {
                if (i == UKURAN_KOTA / 2 && j == UKURAN_KOTA / 2) gambarAlunAlun(ukAlunAlun);
                continue;
            }

            // unntuk area samping kantor polisi
            if (i == UKURAN_KOTA / 2 && j == (UKURAN_KOTA / 2) - 2) { 
                gambarKantorPolisi(x, z); 
                continue; 
            }

            // untuk trotoar
            gambarBagianModel(x, 0.3f, z, UKURAN_GEDUNG + 10, 0.8f, UKURAN_GEDUNG + 10, 0.45f, 0.45f, 0.45f);

            // untuk menempatkan lampu lalu lintas dibeberapa area
            if (i % 2 == 0 && j % 2 == 0) gambarLampuLaluLintas(x + 70, z + 70, 45.0f);

            // untuk menempatkan gedung secara acak
            gambarGedungModel(x, z, i * 1000 + j);
        }
    }
}


void inisialisasi() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); 
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_LIGHTING);   
    glEnable(GL_LIGHT0);     
    glEnable(GL_COLOR_MATERIAL); 
    glEnable(GL_NORMALIZE);   

    // parmeter untuk matahari
    GLfloat amb[] = { 0.0f, 0.0f, 0.0f, 0.0f }; 
    GLfloat dif[] = { 1.0f, 1.0f, 0.0f, 1.0f }; 
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    
    // parameter cahaya 1 yang bersifat spotlight
    GLfloat spot_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_dif);

    // parameter cahaya 2 yang bersifat floodlight
    GLfloat flood_dif[] = { 0.0f, 0.0f, 0.0f, 0.0f }; 
    glLightfv(GL_LIGHT2, GL_DIFFUSE, flood_dif);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void tampilan() {
    // untuk menghitung kecerahan langit berdasarkan siklus
    float kecerahan = sin(waktuSiklus * PI / 180.0f);
    if (kecerahan < -0.2f) kecerahan = -0.2f; 

    // untuk mengubah warna langit secara dianmis
    glClearColor(0.5f * (kecerahan + 0.2f), 0.8f * (kecerahan + 0.2f), 1.0f * (kecerahan + 0.2f), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    gluPerspective(50.0f, (float)1280 / 720, 1.0f, 20000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // logika untuk pemilihan kamera
    if (modeKameraMobil) {
        float rad = pMobilSudut * PI / 180.0f;
        // untuk menempatkan kamera dibelakang mobil dengan sedikit ketinggian
        gluLookAt(pMobilX - sin(rad) * 100.0f, 30.0f, pMobilZ - cos(rad) * 100.0f,
                  pMobilX, 3.0f, pMobilZ, 0.0f, 1.0f, 0.0f);
    } else {
        // untuk kamera bebas kesana-kemari
        float yRad = kameraYaw * PI / 180.0f, pRad = kameraPitch * PI / 180.0f;
        float ax = kameraPosX + sin(yRad) * cos(pRad);
        float ay = kameraPosY - sin(pRad);
        float az = kameraPosZ - cos(yRad) * cos(pRad);
        gluLookAt(kameraPosX, kameraPosY, kameraPosZ, ax, ay, az, 0, 1, 0);
    }

    // untuk menggerakan posisi matahari dan bulan sesuai waktu siklus
    float sunX = cos(waktuSiklus * PI / 180.0f) * 5000.0f;
    float sunY = sin(waktuSiklus * PI / 180.0f) * 5000.0f;
    GLfloat posCahaya[] = { sunX, sunY, -2000.0f, 0.0f }; 
    glLightfv(GL_LIGHT0, GL_POSITION, posCahaya);

    // untuk pencahayaan spotlight yang berpusat pada puncak tugu
    if (lampuSpotNyala) {
        glEnable(GL_LIGHT1);
        GLfloat pSpot[] = { 0.0f, 175.0f, 0.0f, 1.0f }; 
        GLfloat aSpot[] = { 0.0f, -2.0f, .0f }; 
        glLightfv(GL_LIGHT1, GL_POSITION, pSpot);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, aSpot);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, sudutLampuSpot); 
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.001f); 
    } else glDisable(GL_LIGHT1);

    // untuk penerangan saat malam
    glEnable(GL_LIGHT2);
    GLfloat pFlood[] = { 0.0f, 1000.0f, 0.0f, 1.0f }; 
    glLightfv(GL_LIGHT2, GL_POSITION, pFlood);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, jangkauanCahaya); 
    
    // untuk membuat objek matahri dan bulan
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(sunX, sunY, -2000.0f);
    if(sunY > 0) glColor3f(1,1,0); else glColor3f(0.9f,0.9f,1); 
    glutSolidSphere(200, 30, 30);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // untuk menggambar keseluruhan kota
    gambarKotaModel();
    gambarMobil(pMobilX, pMobilZ, pMobilSudut, 12345);

    glutSwapBuffers(); // agar hasil render tertampilkan di window
}

void perbarui() {
    // input untuk rotate, scale, dan cahaya spotlight
    if (tombol['r']) { rotasiTugu += 2.0f; } // rotasi untuk tugu
    //  skala pada kantor polisi 
if (tombol['+']) { 
    // untuk memberikan aturan menambahkan skala hanya jika masih di bawah batas maksimal
    if (skalaPolisi < 3.0f) { 
        skalaPolisi += 0.02f; 
    }
}

if (tombol['_']) { 
    // menambahkan aturan mengurangi skala hanya jika masih di atas batas minimal
    if (skalaPolisi > 0.5f) { 
        skalaPolisi -= 0.02f; 
    }
}
    if (tombol['1']) { sudutLampuSpot += 0.5f; if (sudutLampuSpot > 80) sudutLampuSpot = 80; } // untuk melebarkan sorotan
    if (tombol['0']) { sudutLampuSpot -= 0.2f; if (sudutLampuSpot < 1) sudutLampuSpot = 1; } // untuk menyempitkan sorotan

    // untuk pergerakan kamera kesana kemari
    if (!modeKameraMobil) {
         float rad = kameraYaw * PI / 180.0f;
        if (tombol['w']) { kameraPosX += sin(rad) * kec; kameraPosZ -= cos(rad) * kec; } 
        if (tombol['s']) { kameraPosX -= sin(rad) * kec; kameraPosZ += cos(rad) * kec; } 
        if (tombol['a']) { kameraPosX -= cos(rad) * kec; kameraPosZ -= sin(rad) * kec; } 
        if (tombol['d']) { kameraPosX += cos(rad) * kec; kameraPosZ += sin(rad) * kec; } 
        if (tombol[' ']) kameraPosY += kec; 
        if (tombol['c']) kameraPosY -= kec; 
    }

    // untuk pergerakan mobil
    if (tombolSpesial[GLUT_KEY_UP]) { if (pMobilKecepatan < pMobilSpeedMaks) pMobilKecepatan += 0.04f; } // untuk gasin bos
    else if (tombolSpesial[GLUT_KEY_DOWN]) { if (pMobilKecepatan > -1) pMobilKecepatan -= 0.04f; } // buat ngedrift atau rem
    else pMobilKecepatan *= 0.92f; // untuk perlambatan

    // agar mobil bisa belok dengan ketentuan hanya jika mobil sedang dijalankan
    if (abs(pMobilKecepatan) > 0.1f) {
        if (tombolSpesial[GLUT_KEY_LEFT]) pMobilSudut += 3.0f; 
        if (tombolSpesial[GLUT_KEY_RIGHT]) pMobilSudut -= 3.0f; 
    }

    // untuk posisi mobil saat tabrakan 
    float radM = pMobilSudut * PI / 180.0f;
    float xN = pMobilX + sin(radM) * pMobilKecepatan;
    float zN = pMobilZ + cos(radM) * pMobilKecepatan;

    if (cekTabrakan(xN, zN)) pMobilKecepatan = -pMobilKecepatan * 0.0f; // untuk efek terpental jika nabrak
    else { pMobilX = xN; pMobilZ = zN; }

    // untuk jalnnya waktu siklus matahari
    if (otomatisWaktu) { waktuSiklus += kecepatanWaktu; if (waktuSiklus > 360) waktuSiklus -= 360; }
    
    glutPostRedisplay(); // untuk mengulang layar
}

// fungsi keyboard
void keyboard(unsigned char k, int x, int y) {
    tombol[k] = true;
    if (k == 27) exit(0); // tombol ESC untuk keluar program
    if (k == 'o' || k == 'O') otomatisWaktu = !otomatisWaktu; // untuk memberhentikan siklus waktu matahari(jawa rudo)
    if (k == 'g' || k == 'G') { waktuSiklus = (waktuSiklus > 0 && waktuSiklus < 180) ? 270 : 90; } // siklus agar bisa berganti siang dan malam
    if (k == 'l' || k == 'L') lampuSpotNyala = !lampuSpotNyala; // saklar untuk pencahayaan tugu
    if (k == 'v' || k == 'V') modeKameraMobil = !modeKameraMobil; // untuk gonta-ganti kamera
}

// saat tombol keyboard dilepas
void keyboardLepas(unsigned char k, int x, int y) { tombol[k] = false; }
// saat tombol khusus (Panah) ditekan
void special(int k, int x, int y) { if (k < 256) tombolSpesial[k] = true; }
// saat tombol khusus dilepas
void specialUp(int k, int x, int y) { if (k < 256) tombolSpesial[k] = false; }

// pergerakan mouse untuk rotasi kamera bebas
void mouse(int x, int y) {
    if (modeKameraMobil || !mouseAktif) return; 
    kameraYaw += (x - mouseTerakhirX) * 0.2f;
    kameraPitch += (y - mouseTerakhirY) * 0.2f;
    // membatasi sudut pandang agar tidak terbalik
    if (kameraPitch > 89) kameraPitch = 89;
    if (kameraPitch < -89) kameraPitch = -89;
    mouseTerakhirX = x; mouseTerakhirY = y;
}

// klik mouse untuk aktivasi kontrol kamera
void klik(int b, int s, int x, int y) {
    if (b == GLUT_LEFT_BUTTON && s == GLUT_DOWN) {
        mouseAktif = !mouseAktif;
        mouseTerakhirX = x; mouseTerakhirY = y;
        // untuk menyembunyikan kursor saat mengontrol kamera
        glutSetCursor(mouseAktif ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(1280, 720); // Resolusi window
    glutCreateWindow("Kelompok 1 - TB"); 

    inisialisasi(); 

    glutDisplayFunc(tampilan);
    glutIdleFunc(perbarui);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardLepas);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(klik);
    glutPassiveMotionFunc(mouse);
    
    glutMainLoop(); 
    return 0;
}
