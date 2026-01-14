#include <GL/freeglut.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

const float PI = 3.14159265f;

// --- KAMERA ---
float kameraPosisiX = 0.0f, kameraPosisiY = 300.0f, kameraPosisiZ = 800.0f;
float kameraYaw = 0.0f, kameraPitch = -20.0f;
bool modeKameraMobil = false; 

// --- VARIABEL OBJEK (MANIPULASI MANUAL) ---
float skalaTugu = 1.0f;
float skalaPolisi = 1.0f; 
float rotasiTugu = 0.0f;  

// --- VARIABEL MOBIL PLAYER ---
float pMobilX = 0.0f;           
float pMobilZ = 500.0f;         
float pMobilSudut = 180.0f;       
float pMobilKecepatan = 0.0f;   
float pMobilSpeedMaks = 2.0f;  

// --- VARIABEL SIKLUS SIANG-MALAM ---
float waktuSiklus = 0.0f;       
float kecepatanWaktu = 0.2f;    
bool otomatisWaktu = true;      

// --- VARIABEL SPOTLIGHT ---
bool lampuSpotNyala = false;

// --- INPUT STATE ---
bool tombol[256] = { false };
bool mouseAktif = false;
int mouseTerakhirX, mouseTerakhirY;

// --- PENGATURAN KOTA ---
const int UKURAN_KOTA = 6;
const int UKURAN_BLOK = 120;
const float UKURAN_BANGUNAN = 120.0f;
const float LEBAR_JALAN = 100.0f;

// --- FUNGSI DASAR GAMBAR ---
void gambarBagianRoblox(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b, bool neon = false) {
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

// --- OBJEK-OBJEK KOTA ---
// Oleh fauzan
void gambarTugu() {
    GLUquadric* q = gluNewQuadric();
    
    glPushMatrix();
    glRotatef(rotasiTugu, 0, 1, 0); 
    glScalef(skalaTugu, skalaTugu, skalaTugu);
    
    // Alas Bertingkat
    gambarBagianRoblox(0, 2.3f, 0, 60, 4, 60, 0.2f, 0.2f, 0.2f);
    gambarBagianRoblox(0, 6.3f, 0, 45, 6, 45, 0.7f, 0.7f, 0.7f);
    gambarBagianRoblox(0, 12.3f, 0, 30, 8, 30, 0.4f, 0.4f, 0.4f);

    // Badan Tugu
    glPushMatrix();
    glTranslatef(0, 20.0f, 0);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.9f, 0.9f, 0.9f);
    gluCylinder(q, 10.0f, 5.0f, 150.0f, 16, 1); 
    glPopMatrix();

    // Puncak Kristal
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

// Oleh Fauzan
void gambarAlunAlun(float ukuran) {
    GLUquadric* q = gluNewQuadric();

    // Lantai Dasar
    glPushMatrix();
    glTranslatef(0, 0.1f, 0);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.2f, 0.6f, 0.2f);
    gluDisk(q, 0, ukuran * 0.3f, 64, 1);
    glPopMatrix();

    // Area Upacara
    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.3f, 0.3f, 0.3f);
    gluDisk(q, 0, ukuran * 0.2f, 64, 1);
    glTranslatef(0, 0, 0.1f);
    glColor3f(0.5f, 0.5f, 0.5f);
    gluDisk(q, 0, ukuran * 0.1f, 64, 1);
    glPopMatrix();

    gambarTugu();

    // Pohon
    for (int i = 0; i < 24; i++) {
        float sudut = i * (360.0f / 24.0f) * 3.14159f / 180.0f;
        float jarak = ukuran * 0.25f;
        float px = cos(sudut) * jarak;
        float pz = sin(sudut) * jarak;

        glPushMatrix();
        glTranslatef(px, 0.0f, pz);
        glRotatef(-90, 1, 0, 0);
        glColor3f(0.35f, 0.2f, 0.1f);
        gluCylinder(q, 3.5f, 3.0f, 35.0f, 10, 1);
        glPopMatrix();

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

void gambarGedungRoblox(float x, float z, int benih) {
    srand(benih);
    if (rand() % 10 > 7) return; 

    float tinggi = 120.0f + (rand() % 280);
    float lebar = 70.0f + (rand() % 30);
    float r = (rand() % 100 + 50) / 255.0f;
    float g = (rand() % 100 + 50) / 255.0f;
    float b = (rand() % 100 + 50) / 255.0f;

    gambarBagianRoblox(x, 0.5f, z, lebar, tinggi, lebar, r, g, b);

    // Pintu & Jendela
    float lebarPintuTotal = lebar * 0.4f;
    float tinggiPintu = 30.0f;
    float posZ_Pintu = z + (lebar / 2.0f) + 0.5f;

    gambarBagianRoblox(x, 0.5f, posZ_Pintu, lebarPintuTotal + 6, tinggiPintu + 3, 2, 0.1f, 0.1f, 0.1f);
    
    // Gagang Pintu
    for (int i = -1; i <= 1; i += 2) {
        gambarBagianRoblox(x + (i * 3.0f), 12.0f, posZ_Pintu + 1.2f, 
                           1.5f, 8.0f, 1.5f, 
                           1.0f, 0.8f, 0.0f); 
    }
    
    // Jendela Loop
    float ukKaca = 12.0f;
    float jarakLantai = 25.0f;
    float offsetKaca = lebar / 2.0f + 0.6f;
    float kr = 0.4f, kg = 0.9f, kb = 1.0f;

    for (float h = tinggiPintu + 15.0f; h < tinggi - 15.0f; h += jarakLantai) {
        for (int sisi = 0; sisi < 4; sisi++) {
            for (float horiz = -1.0f; horiz <= 1.0f; horiz += 2.0f) {
                float posH = horiz * (lebar * 0.25f);
                glPushMatrix();
                glTranslatef(x, h, z);
                glRotatef(sisi * 90.0f, 0, 1, 0);
                gambarBagianRoblox(posH, 0, offsetKaca + 0.5f, ukKaca, ukKaca, 1.0f, kr, kg, kb, true);
                glPopMatrix();
            }
        }
    }
    gambarBagianRoblox(x, tinggi, z, lebar + 8, 4, lebar + 8, 0.15f, 0.15f, 0.15f);
}

// Oleh Andhika
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

    gambarBagianRoblox(0, 55, 10, 4, 4, 20, 0.1f, 0.1f, 0.1f);
    gambarBagianRoblox(0, 49, 21, 5, 16, 4, 0.1f, 0.1f, 0.1f);

    glPushMatrix();
    glTranslatef(0, 62.0f, 23.5f); 
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(2.0f, 16, 16); 
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 57.0f, 23.5f);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.7f, 0.0f);
    glutSolidSphere(2.0f, 16, 16);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 52.0f, 23.5f);
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 1.0f, 0.0f);
    glutSolidSphere(2.0f, 16, 16);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    gluDeleteQuadric(q);
    glPopMatrix();
}

// Oleh Albar
void gambarMobil(float x, float z, float rotasi, int benih) {
    srand(benih);
    float r = (rand() % 200 + 55) / 255.0f;
    float g = (rand() % 200 + 55) / 255.0f;
    float b = (rand() % 200 + 55) / 255.0f;

    glPushMatrix();
    glTranslatef(x, 3.0f, z);       
    glRotatef(rotasi, 0, 1, 0);     

    gambarBagianRoblox(0, 3.0f, 0, 22, 6, 46, r, g, b);
    gambarBagianRoblox(0, 9.0f, -4.0f, 20, 7, 22, r, g, b);

    gambarBagianRoblox(0, 9.5f, 7.1f, 19, 5, 0.5f, 0.5f, 1.0f, 1.0f, true);
    gambarBagianRoblox(0, 9.5f, -15.1f, 19, 5, 0.5f, 0.5f, 1.0f, 1.0f, true);
    
    glPushMatrix();
    glTranslatef(7.0f, 6.0f, 23.5f); 
    glDisable(GL_LIGHTING); 
    glColor3f(1.0f, 0.9f, 0.0f); 
    glutSolidSphere(2.2f, 16, 16); 
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-7.0f, 6.0f, 23.5f);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.0f);
    glutSolidSphere(2.2f, 16, 16);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    gambarBagianRoblox(0.0f, 5.0f, 23.1f, 8, 2.5f, 0.5f, 0.2f, 0.2f, 0.2f);

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

// Oleh Zidane
void gambarKantorPolisi(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(0, 0, 1, 0); 
    glScalef(skalaPolisi, skalaPolisi, skalaPolisi);

    gambarBagianRoblox(0, 0, 0, 240, 5, 160, 0.2f, 0.2f, 0.2f);
    gambarBagianRoblox(0, 5, 0, 220, 80, 140, 0.05f, 0.1f, 0.4f);
    gambarBagianRoblox(0, 45, 0, 222, 10, 142, 1.0f, 1.0f, 1.0f);

    for(int i = -1; i <= 1; i += 2) {
        gambarBagianRoblox(i * 75, 10, 71.1f, 45, 40, 2, 0.1f, 0.1f, 0.1f);
        gambarBagianRoblox(i * 75, 22, 71.3f, 38, 35, 1, 0.4f, 0.9f, 1.0f, true);
    }

    gambarBagianRoblox(0, 5, 75, 120, 85, 20, 0.1f, 0.15f, 0.5f); 
    gambarBagianRoblox(0, 60, 90, 100, 5, 40, 0.3f, 0.3f, 0.3f);  

    for(int i = -1; i <= 1; i += 2) {
        gambarBagianRoblox(i * 18, 5, 86.1f, 30, 48, 2, 0.05f, 0.05f, 0.05f);
        gambarBagianRoblox(i * 18, 8, 86.2f, 22, 42, 1, 0.8f, 1.0f, 1.0f, true);
        gambarBagianRoblox(i * 8, 20, 87.5f, 1.5f, 10, 1.5f, 1.0f, 0.8f, 0.0f);
    }

    gambarBagianRoblox(0, 62, 86.0f, 106, 22, 3, 0.0f, 0.0f, 0.0f); 
    float lx = -42.0f; float ly = 66.0f; float lz = 88.0f; float s = 1.8f; float sp = 14.0f; 
    
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    // P
    gambarBagianRoblox(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly + 12, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly + 6, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx + 7, ly + 8, lz, s, 6, s, 1,1,1);

    lx += sp; // O
    gambarBagianRoblox(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianRoblox(lx + 7, ly, lz, s, 14, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly + 12, lz, 7, s, s, 1,1,1);

    lx += sp; // L
    gambarBagianRoblox(lx, ly, lz, s, 14, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);

    lx += sp; // I
    gambarBagianRoblox(lx + 3.5f, ly, lz, s, 14, s, 1,1,1);

    lx += sp; // S
    gambarBagianRoblox(lx + 3.5f, ly, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly + 7, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx + 3.5f, ly + 13, lz, 7, s, s, 1,1,1);
    gambarBagianRoblox(lx, ly + 8, lz, s, 6, s, 1,1,1);
    gambarBagianRoblox(lx + 7, ly + 2, lz, s, 6, s, 1,1,1);

    lx += sp; // I
    gambarBagianRoblox(lx + 3.5f, ly, lz, s, 14, s, 1,1,1);

    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(-20, 94, 80); 
    glPushMatrix();
    glRotatef(90, 1, 0, 0); 
    GLUquadric *quadric1 = gluNewQuadric();
    glColor3f(0.3f, 0.3f, 0.3f);
    gluCylinder(quadric1, 2.5f, 2.5f, 8.0f, 16, 1);
    gluDeleteQuadric(quadric1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 4.0f, 0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(4.0f, 20, 20);
    glPopMatrix();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(20, 94, 80); 
    glPushMatrix();
    glRotatef(90, 1, 0, 0); 
    GLUquadric *quadric2 = gluNewQuadric();
    glColor3f(0.3f, 0.3f, 0.3f);
    gluCylinder(quadric2, 2.5f, 2.5f, 8.0f, 16, 1);
    gluDeleteQuadric(quadric2);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 4.0f, 0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glutSolidSphere(4.0f, 20, 20);
    glPopMatrix();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(75, 0, -30); 
    
    glPushMatrix();
    glTranslatef(0, 2.5f, 0);
    glScalef(45.0f, 5.0f, 45.0f);
    glColor3f(0.15f, 0.15f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 85.0f, 0); 
    glRotatef(-90, 1, 0, 0); 
    GLUquadric *quadric3 = gluNewQuadric();
    glColor3f(0.15f, 0.15f, 0.2f);
    gluCylinder(quadric3, 10.0f, 8.0f, 160.0f, 16, 1);
    gluDeleteQuadric(quadric3);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 200.0f, 0); 
    glScalef(30.0f, 25.0f, 30.0f);
    glColor3f(0.4f, 0.9f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 225.0f, 0); 
    glColor3f(0.2f, 0.5f, 0.8f);
    glutSolidSphere(12.0f, 20, 20);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 237.0f, 0); 
    glRotatef(-90, 1, 0, 0); 
    GLUquadric *quadric4 = gluNewQuadric();
    glColor3f(0.3f, 0.3f, 0.3f);
    gluCylinder(quadric4, 0.5f, 0.5f, 20.0f, 8, 1);
    gluDeleteQuadric(quadric4);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 257.0f, 0); 
    glColor3f(1.0f, 1.0f, 0.8f);
    glutSolidSphere(2.0f, 12, 12);
    glPopMatrix();
    
    glPopMatrix(); 

    glPopMatrix(); 
}

void gambarKotaRoblox() {
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;

    gambarBagianRoblox(0, -2.0f, 0, 1500, 2, 1500, 0.2f, 0.2f, 0.2f); // Tanah

    float ukAlunAlun = langkah * 3.0f;

    for (int i = 0; i < UKURAN_KOTA; i++) {
        for (int j = 0; j < UKURAN_KOTA; j++) {
            float x = -posisiAwal + i * langkah + langkah / 2.0f;
            float z = -posisiAwal + j * langkah + langkah / 2.0f;

            if (abs(x) < ukAlunAlun / 2.0f && abs(z) < ukAlunAlun / 2.0f) {
                if (i == UKURAN_KOTA / 2 && j == UKURAN_KOTA / 2) {
                    gambarAlunAlun(ukAlunAlun);
                }
                continue;
            }

            if (i == UKURAN_KOTA / 2 && j == (UKURAN_KOTA / 2) - 2) { 
                gambarKantorPolisi(x, z); 
                continue; 
            }

            gambarBagianRoblox(x, 0.3f, z, UKURAN_BANGUNAN + 10, 0.8f, UKURAN_BANGUNAN + 10, 0.45f, 0.45f, 0.45f);

            if (i % 2 == 0 && j % 2 == 0) {
                gambarLampuLaluLintas(x + 70, z + 70, 45.0f);
            }

            gambarGedungRoblox(x, z, i * 1000 + j);
        }
    }
}

// --- FUNGSI DETEKSI TABRAKAN ---
bool cekTabrakan(float xBaru, float zBaru) {
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN; 
    float totalPeta = UKURAN_KOTA * langkah;
    float batasPeta = totalPeta / 2.0f;
    float batasTepi = batasPeta - 60.0f; 

    // 1. Cek Batas Pinggir Peta
    if (xBaru < -batasTepi || xBaru > batasTepi ||
        zBaru < -batasTepi || zBaru > batasTepi) {
        return true; 
    }

    float jarakDariPusat = sqrt(xBaru * xBaru + zBaru * zBaru);
    float radiusAlunAlun = 200.0f; 

    if (jarakDariPusat < radiusAlunAlun) {
        return true; 
    }

    // Cek Tabrakan dengan Gedung
    int gridI = (int)((xBaru + batasPeta) / langkah);
    int gridJ = (int)((zBaru + batasPeta) / langkah);

    // Pastikan indeks grid valid
    if (gridI >= 0 && gridI < UKURAN_KOTA && gridJ >= 0 && gridJ < UKURAN_KOTA) {
       
        bool areaAlunAlun = (gridI == 2 || gridI == 3) && (gridJ == 2 || gridJ == 3);

        if (!areaAlunAlun) {
            float pusatGedungX = -batasPeta + gridI * langkah + langkah / 2.0f;
            float pusatGedungZ = -batasPeta + gridJ * langkah + langkah / 2.0f;

            float dx = xBaru - pusatGedungX;
            float dz = zBaru - pusatGedungZ;
            float jarakTabrak = 65.0f; 

            if (abs(dx) < jarakTabrak && abs(dz) < jarakTabrak) {
                return true; 
            }
        }
    }

    return false;
}

void inisialisasi() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    GLfloat matahari_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat matahari_diffuse[] = { 1.0f, 1.0f, 0.9f, 1.0f };
    GLfloat matahari_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, matahari_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, matahari_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, matahari_specular);
    
    GLfloat spot_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat spot_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spot_specular);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void tampilan() {
    float kecerahan = sin(waktuSiklus * PI / 180.0f);
    if (kecerahan < -0.2f) kecerahan = -0.2f; 

    float skyR = 0.5f * (kecerahan + 0.2f);
    float skyG = 0.8f * (kecerahan + 0.2f);
    float skyB = 1.0f * (kecerahan + 0.2f);
    glClearColor(skyR, skyG, skyB, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (float)1280 / 720, 1.0f, 20000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (modeKameraMobil) {
        float rad = pMobilSudut * PI / 180.0f;
        float jarak = 100.0f;
        float tinggi = 55.0f;
        float cX = pMobilX - sin(rad) * jarak;
        float cZ = pMobilZ - cos(rad) * jarak;
        float cY = 3.0f + tinggi;

        gluLookAt(cX, cY, cZ,
                  pMobilX, 3.0f, pMobilZ,
                  0.0f, 1.0f, 0.0f);
    } else {
        float yawRad = kameraYaw * PI / 180.0f;
        float pitchRad = kameraPitch * PI / 180.0f;
        float arahX = kameraPosisiX + sin(yawRad) * cos(pitchRad);
        float arahY = kameraPosisiY - sin(pitchRad);
        float arahZ = kameraPosisiZ - cos(yawRad) * cos(pitchRad);
        gluLookAt(kameraPosisiX, kameraPosisiY, kameraPosisiZ, arahX, arahY, arahZ, 0, 1, 0);
    }

    float orbitRadius = 5000.0f;
    float sunX = cos(waktuSiklus * PI / 180.0f) * orbitRadius;
    float sunY = sin(waktuSiklus * PI / 180.0f) * orbitRadius;
    float sunZ = -2000.0f;

    GLfloat posCahaya[] = { sunX, sunY, sunZ, 0.0f }; 
    glLightfv(GL_LIGHT0, GL_POSITION, posCahaya);

    if (lampuSpotNyala) {
        glEnable(GL_LIGHT1);
        float langkah = UKURAN_BANGUNAN + LEBAR_JALAN; 
        float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f; 
        
        float alunX = -posisiAwal + 3 * langkah + langkah / 2.0f;
        float alunZ = -posisiAwal + 3 * langkah + langkah / 2.0f;
        float polX = -posisiAwal + 3 * langkah + langkah / 2.0f;
        float polZ = -posisiAwal + 1 * langkah + langkah / 2.0f;
        
       
        GLfloat posisiSpot[] = { polX + 75.0f, 200.0f, polZ - 30.0f, 1.0f };
        GLfloat arahSpot[] = { alunX - (polX + 75.0f), -200.0f, alunZ - (polZ - 30.0f) };

        glLightfv(GL_LIGHT1, GL_POSITION, posisiSpot);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, arahSpot);
        
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0f);  
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5f);
    } else {
        glDisable(GL_LIGHT1);
    }
    
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(sunX, sunY, sunZ);
    if (sunY > 0) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glutSolidSphere(200.0f, 30, 30);
    } else {
        glColor3f(0.9f, 0.9f, 1.0f);
        glutSolidSphere(150.0f, 30, 30);
    }
    glPopMatrix();
    glEnable(GL_LIGHTING);

    gambarKotaRoblox();
    gambarMobil(pMobilX, pMobilZ, pMobilSudut, 12345);

    glutSwapBuffers();
}

void perbarui() {
    if (tombol['r']) {
        rotasiTugu += 2.0f; 
        if (rotasiTugu > 360.0f) rotasiTugu -= 360.0f;
    }

    if (tombol['+']) {
        skalaPolisi += 0.02f; 
    }
    if (tombol['_']) { 
        skalaPolisi -= 0.02f; 
        if (skalaPolisi < 0.1f) skalaPolisi = 0.1f; 
    }

    if (!modeKameraMobil) {
        float kecepatanKamera = 15.0f;
        float radKamera = kameraYaw * PI / 180.0f;

        if (tombol['w']) { kameraPosisiX += sin(radKamera) * kecepatanKamera; kameraPosisiZ -= cos(radKamera) * kecepatanKamera; }
        if (tombol['s']) { kameraPosisiX -= sin(radKamera) * kecepatanKamera; kameraPosisiZ += cos(radKamera) * kecepatanKamera; }
        if (tombol['a']) { kameraPosisiX -= cos(radKamera) * kecepatanKamera; kameraPosisiZ -= sin(radKamera) * kecepatanKamera; }
        if (tombol['d']) { kameraPosisiX += cos(radKamera) * kecepatanKamera; kameraPosisiZ += sin(radKamera) * kecepatanKamera; }
        if (tombol[' ']) kameraPosisiY += kecepatanKamera;
        if (tombol['c']) kameraPosisiY -= kecepatanKamera;
    }

    if (tombol['i']) { 
        if (pMobilKecepatan < pMobilSpeedMaks) pMobilKecepatan += 0.04f; 
    } else if (tombol['k']) { 
        if (pMobilKecepatan > -pMobilSpeedMaks/2) pMobilKecepatan -= 0.04f;
    } else {
        pMobilKecepatan *= 0.92f; 
    }

    if (abs(pMobilKecepatan) > 0.1f) {
        if (tombol['j']) pMobilSudut += 3.0f; 
        if (tombol['l']) pMobilSudut -= 3.0f; 
    }

    float radMobil = pMobilSudut * PI / 180.0f;
    float xSelanjutnya = pMobilX + sin(radMobil) * pMobilKecepatan;
    float zSelanjutnya = pMobilZ + cos(radMobil) * pMobilKecepatan;

    if (cekTabrakan(xSelanjutnya, zSelanjutnya)) {
        pMobilKecepatan = -pMobilKecepatan * 0.5f; 
    } else {
        pMobilX = xSelanjutnya;
        pMobilZ = zSelanjutnya;
    }

    if (otomatisWaktu) {
        waktuSiklus += kecepatanWaktu;
        if (waktuSiklus > 360.0f) waktuSiklus -= 360.0f;
    }
   
    glutPostRedisplay();
}

void keyboard(unsigned char k, int x, int y) {
    tombol[k] = true;
    if (k == 27) exit(0); 
    
    if (k == 'o' || k == 'O') otomatisWaktu = !otomatisWaktu; 

    if (k == 'm' || k == 'M') {
        if (waktuSiklus > 0 && waktuSiklus < 180) waktuSiklus = 270.0f; 
        else waktuSiklus = 90.0f;  
    }

    if (k == '1') lampuSpotNyala = true;  
    if (k == '0') lampuSpotNyala = false; 

    if (k == 'v' || k == 'V') {
        modeKameraMobil = !modeKameraMobil;
        if(modeKameraMobil) std::cout << "Mode Kamera: Mobil" << std::endl;
        else std::cout << "Mode Kamera: Bebas" << std::endl;
    }
}

void keyboardLepas(unsigned char k, int x, int y) { tombol[k] = false; }

void gerakMouse(int x, int y) {
    if (modeKameraMobil) return; 

    if (mouseAktif) {
        kameraYaw += (x - mouseTerakhirX) * 0.2f;
        kameraPitch += (y - mouseTerakhirY) * 0.2f;
        if (kameraPitch > 89) kameraPitch = 89;
        if (kameraPitch < -89) kameraPitch = -89;
        mouseTerakhirX = x; mouseTerakhirY = y;
    }
}

void klikMouse(int b, int s, int x, int y) {
    if (b == GLUT_LEFT_BUTTON && s == GLUT_DOWN) {
        mouseAktif = !mouseAktif;
        mouseTerakhirX = x; mouseTerakhirY = y;
        glutSetCursor(mouseAktif ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Simulasi Kota 3D - Full Lighting"); 

    inisialisasi();

    glutDisplayFunc(tampilan);
    glutIdleFunc(perbarui);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardLepas);
    glutMouseFunc(klikMouse);
    glutPassiveMotionFunc(gerakMouse);
    glutMotionFunc(gerakMouse); 

    glutMainLoop();
    return 0;
}
