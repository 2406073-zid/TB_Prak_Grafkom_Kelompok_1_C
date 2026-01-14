#include <GL/glut.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

#define PI 3.14159265f

using namespace std;

// --- Konfigurasi Kamera ---
float kameraPosisiX = 0.0f, kameraPosisiY = 800.0f, kameraPosisiZ = 1200.0f;
float kameraYaw = 45.0f, kameraPitch = -40.0f;
bool tombol[256] = {false};
bool mouseAktif = false;
int mouseTerakhirX, mouseTerakhirY;
float waktuGlobal = 12.0f; // Jam 12 siang
bool modeMalam = false;

// --- Konfigurasi Kota ---
const int UKURAN_KOTA = 15;              // Dikurangi untuk performa Dev-C++
const int UKURAN_BLOK = 80;              
const float UKURAN_BANGUNAN = 100.0f;    
const float LEBAR_JALAN = 80.0f;         
const float LEBAR_TROTOAR = 20.0f;

// --- Struktur Data ---
struct Mobil {
    float posX, posZ;
    float kecepatan;
    float arah;
    float r, g, b;
    float waktuLampuRem;
    int rute;
    bool diPersimpangan;
    bool berhentiLampuMerah;
};

struct LampuLaluLintas {
    float posX, posZ;
    float rotasi;
    int status;
    float waktuSiklus;
    float timer;
};

struct Bangunan {
    float posX, posZ;
    float tinggi, lebar, kedalaman;
    float r, g, b;
    bool memilikiLampuJendela;
    int jenis;
};

vector<Mobil> daftarMobil;
vector<LampuLaluLintas> daftarLampu;
vector<Bangunan> daftarBangunan;
int jumlahMobil = 30;
float siklusLampu = 10.0f;

// --- Fungsi Bantuan ---
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
}

// --- Inisialisasi Kota ---
void inisialisasiKota() {
    srand(time(NULL));
    
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN + LEBAR_TROTOAR*2;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;
    
    // Bangunan
    for (int i = 0; i < UKURAN_KOTA; i++) {
        for (int j = 0; j < UKURAN_KOTA; j++) {
            float x = -posisiAwal + i * langkah + langkah / 2.0f;
            float z = -posisiAwal + j * langkah + langkah / 2.0f;
            
            // Lampu lalu lintas di persimpangan tertentu
            if (i % 2 == 0 && j % 2 == 0 && i > 0 && i < UKURAN_KOTA-1 && j > 0 && j < UKURAN_KOTA-1) {
                LampuLaluLintas lampu;
                lampu.posX = x;
                lampu.posZ = z;
                lampu.rotasi = 45.0f;
                lampu.status = (i + j) % 3;
                lampu.waktuSiklus = siklusLampu;
                lampu.timer = randomFloat(0, siklusLampu);
                daftarLampu.push_back(lampu);
            }
            
            // Bangunan
            Bangunan bangunan;
            bangunan.posX = x;
            bangunan.posZ = z;
            
            int zona = (i + j) % 4;
            bangunan.jenis = zona;
            
            switch (zona) {
                case 0: // Perkantoran
                    bangunan.tinggi = 150.0f + randomFloat(0, 200.0f);
                    bangunan.lebar = 70.0f + randomFloat(0, 20.0f);
                    bangunan.kedalaman = bangunan.lebar;
                    bangunan.r = randomFloat(0.6f, 0.8f);
                    bangunan.g = randomFloat(0.7f, 0.9f);
                    bangunan.b = randomFloat(0.8f, 1.0f);
                    bangunan.memilikiLampuJendela = true;
                    break;
                case 1: // Perumahan
                    bangunan.tinggi = 40.0f + randomFloat(0, 40.0f);
                    bangunan.lebar = 50.0f + randomFloat(0, 15.0f);
                    bangunan.kedalaman = bangunan.lebar * 0.8f;
                    bangunan.r = randomFloat(0.7f, 0.9f);
                    bangunan.g = randomFloat(0.6f, 0.8f);
                    bangunan.b = randomFloat(0.5f, 0.7f);
                    bangunan.memilikiLampuJendela = (rand() % 100) > 50;
                    break;
                case 2: // Komersial
                    bangunan.tinggi = 80.0f + randomFloat(0, 100.0f);
                    bangunan.lebar = 60.0f + randomFloat(0, 30.0f);
                    bangunan.kedalaman = bangunan.lebar * 1.2f;
                    bangunan.r = randomFloat(0.8f, 1.0f);
                    bangunan.g = randomFloat(0.7f, 0.9f);
                    bangunan.b = randomFloat(0.6f, 0.8f);
                    bangunan.memilikiLampuJendela = true;
                    break;
                case 3: // Pemerintahan
                    bangunan.tinggi = 100.0f + randomFloat(0, 150.0f);
                    bangunan.lebar = 80.0f + randomFloat(0, 30.0f);
                    bangunan.kedalaman = bangunan.lebar * 0.9f;
                    bangunan.r = randomFloat(0.9f, 1.0f);
                    bangunan.g = randomFloat(0.8f, 0.9f);
                    bangunan.b = randomFloat(0.7f, 0.8f);
                    bangunan.memilikiLampuJendela = true;
                    break;
            }
            
            daftarBangunan.push_back(bangunan);
        }
    }
    
    // Mobil
    for (int i = 0; i < jumlahMobil; i++) {
        Mobil mobil;
        mobil.rute = rand() % 2;
        
        if (mobil.rute == 0) {
            mobil.posX = -posisiAwal + randomFloat(0, UKURAN_KOTA * langkah);
            mobil.posZ = -posisiAwal + (rand() % UKURAN_KOTA) * langkah + langkah/2;
            mobil.arah = (rand() % 2) ? 0.0f : 180.0f;
        } else {
            mobil.posX = -posisiAwal + (rand() % UKURAN_KOTA) * langkah + langkah/2;
            mobil.posZ = -posisiAwal + randomFloat(0, UKURAN_KOTA * langkah);
            mobil.arah = (rand() % 2) ? 90.0f : 270.0f;
        }
        
        mobil.kecepatan = 2.0f + randomFloat(0, 4.0f);
        mobil.r = randomFloat(0.2f, 1.0f);
        mobil.g = randomFloat(0.2f, 1.0f);
        mobil.b = randomFloat(0.2f, 1.0f);
        mobil.waktuLampuRem = 0.0f;
        mobil.diPersimpangan = false;
        mobil.berhentiLampuMerah = false;
        
        daftarMobil.push_back(mobil);
    }
}

// --- Fungsi Gambar Primitif ---
void gambarKotak(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b, bool emisif = false) {
    glPushMatrix();
    glTranslatef(x, y + (sy / 2.0f), z);
    glScalef(sx, sy, sz);
    
    if (emisif) {
        float emisi[] = {r, g, b, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisi);
    }
    
    glColor3f(r, g, b);
    glutSolidCube(1.0f);
    
    if (emisif) {
        float noEmisi[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmisi);
    }
    
    glPopMatrix();
}

// --- Fungsi Gambar Kota ---
void gambarJalan() {
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN + LEBAR_TROTOAR*2;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;
    
    // Baseplate
    gambarKotak(0, -5.0f, 0, posisiAwal*4, 10, posisiAwal*4, 0.1f, 0.15f, 0.1f);
    
    // Jalan horizontal dan vertikal
    for (int i = 0; i <= UKURAN_KOTA; i++) {
        for (int j = 0; j <= UKURAN_KOTA; j++) {
            float x = -posisiAwal + i * langkah;
            float z = -posisiAwal + j * langkah;
            
            // Jalan horizontal
            if (i < UKURAN_KOTA) {
                gambarKotak(x + langkah/2, 0.2f, z, LEBAR_JALAN, 0.4f, langkah, 0.2f, 0.2f, 0.2f);
                
                // Marka jalan
                for (float m = -langkah/2 + 20; m < langkah/2 - 20; m += 40.0f) {
                    gambarKotak(x + langkah/2, 0.3f, z + m, 2.0f, 0.1f, 15.0f, 1.0f, 1.0f, 0.0f);
                }
            }
            
            // Jalan vertikal
            if (j < UKURAN_KOTA) {
                gambarKotak(x, 0.2f, z + langkah/2, langkah, 0.4f, LEBAR_JALAN, 0.2f, 0.2f, 0.2f);
                
                // Marka jalan
                for (float m = -langkah/2 + 20; m < langkah/2 - 20; m += 40.0f) {
                    gambarKotak(x + m, 0.3f, z + langkah/2, 15.0f, 0.1f, 2.0f, 1.0f, 1.0f, 0.0f);
                }
            }
            
            // Trotoar
            if (i < UKURAN_KOTA && j < UKURAN_KOTA) {
                gambarKotak(x + langkah/2, 0.1f, z + langkah/2, LEBAR_TROTOAR, 0.2f, LEBAR_TROTOAR, 0.4f, 0.4f, 0.4f);
            }
        }
    }
}

void gambarBangunan(const Bangunan& b) {
    // Badan bangunan
    gambarKotak(b.posX, 0, b.posZ, b.lebar, b.tinggi, b.kedalaman, b.r, b.g, b.b);
    
    // Atap
    float atapR = b.r * 0.7f;
    float atapG = b.g * 0.7f;
    float atapB = b.b * 0.7f;
    gambarKotak(b.posX, b.tinggi + 5.0f, b.posZ, b.lebar + 10.0f, 10.0f, b.kedalaman + 10.0f, atapR, atapG, atapB);
    
    // Jendela (hanya malam hari)
    if (modeMalam && b.memilikiLampuJendela && (rand() % 100 < 70)) {
        int lantai = (int)(b.tinggi / 20.0f);
        if (lantai < 2) lantai = 2;
        
        for (int l = 1; l < lantai; l++) {
            float y = l * (b.tinggi / lantai);
            
            // Empat sisi
            for (int sisi = 0; sisi < 4; sisi++) {
                float jx = b.posX, jz = b.posZ;
                
                switch(sisi) {
                    case 0: jz += b.kedalaman/2 + 0.1f; break;
                    case 1: jz -= b.kedalaman/2 + 0.1f; break;
                    case 2: jx += b.lebar/2 + 0.1f; break;
                    case 3: jx -= b.lebar/2 + 0.1f; break;
                }
                
                glPushMatrix();
                glTranslatef(jx, y, jz);
                if (sisi == 2 || sisi == 3) glRotatef(90, 0, 1, 0);
                
                float emisi[] = {0.9f, 0.8f, 0.3f, 1.0f};
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisi);
                
                glColor3f(0.9f, 0.8f, 0.3f);
                glBegin(GL_QUADS);
                    glVertex3f(-5.0f, -4.0f, 0.0f);
                    glVertex3f(5.0f, -4.0f, 0.0f);
                    glVertex3f(5.0f, 4.0f, 0.0f);
                    glVertex3f(-5.0f, 4.0f, 0.0f);
                glEnd();
                
                float noEmisi[] = {0.0f, 0.0f, 0.0f, 1.0f};
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmisi);
                glPopMatrix();
            }
        }
    }
}

void gambarPosPolisi(float x, float z) {
    // Bangunan
    gambarKotak(x, 0, z, 60.0f, 50.0f, 80.0f, 0.2f, 0.2f, 0.3f);
    
    // Menara
    gambarKotak(x + 30.0f, 25.0f, z - 30.0f, 15.0f, 50.0f, 15.0f, 0.3f, 0.3f, 0.4f);
    
    // Lampu berputar
    if (modeMalam) {
        glPushMatrix();
        glTranslatef(x, 105.0f, z);
        glRotatef(glutGet(GLUT_ELAPSED_TIME) / 10.0f, 0, 1, 0);
        
        // Lampu biru
        float emisiBiru[] = {0.0f, 0.0f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisiBiru);
        glColor3f(0.0f, 0.0f, 1.0f);
        glutSolidSphere(4.0f, 10, 10);
        
        // Lampu merah
        glPushMatrix();
        glRotatef(180.0f, 0, 1, 0);
        float emisiMerah[] = {1.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisiMerah);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(4.0f, 10, 10);
        glPopMatrix();
        
        float noEmisi[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmisi);
        glPopMatrix();
    }
}

void gambarLampuJalan(float x, float z, float rotasi) {
    // Tiang
    gambarKotak(x, 0, z, 1.5f, 35.0f, 1.5f, 0.1f, 0.1f, 0.1f);
    
    // Kepala lampu
    glPushMatrix();
    glTranslatef(x, 33.0f, z);
    glRotatef(rotasi, 0, 1, 0);
    
    gambarKotak(0, 0, 0, 12.0f, 6.0f, 8.0f, 0.05f, 0.05f, 0.05f);
    
    if (modeMalam) {
        float emisi[] = {0.9f, 0.9f, 0.7f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisi);
        
        glColor3f(0.9f, 0.9f, 0.7f);
        glBegin(GL_QUADS);
            glVertex3f(-5.0f, -2.5f, 4.1f);
            glVertex3f(5.0f, -2.5f, 4.1f);
            glVertex3f(5.0f, 2.5f, 4.1f);
            glVertex3f(-5.0f, 2.5f, 4.1f);
        glEnd();
        
        float noEmisi[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmisi);
    }
    
    glPopMatrix();
}

void gambarLampuLaluLintas(const LampuLaluLintas& lampu) {
    glPushMatrix();
    glTranslatef(lampu.posX, 0, lampu.posZ);
    glRotatef(lampu.rotasi, 0, 1, 0);
    
    // Tiang
    gambarKotak(0, 0, 0, 2.5f, 50.0f, 2.5f, 0.15f, 0.15f, 0.15f);
    
    // Kotak lampu
    gambarKotak(0, 45.0f, 6.0f, 10.0f, 25.0f, 6.0f, 0.1f, 0.1f, 0.1f);
    
    // Lampu merah
    float merah = (lampu.status == 0) ? 1.0f : 0.2f;
    gambarKotak(0, 52.0f, 7.1f, 4.0f, 4.0f, 0.1f, merah, 0.0f, 0.0f, lampu.status == 0);
    
    // Lampu kuning
    float kuning = (lampu.status == 1) ? 1.0f : 0.2f;
    gambarKotak(0, 47.0f, 7.1f, 4.0f, 4.0f, 0.1f, kuning, kuning*0.7f, 0.0f, lampu.status == 1);
    
    // Lampu hijau
    float hijau = (lampu.status == 2) ? 1.0f : 0.2f;
    gambarKotak(0, 42.0f, 7.1f, 4.0f, 4.0f, 0.1f, 0.0f, hijau, 0.0f, lampu.status == 2);
    
    glPopMatrix();
}

void gambarMobil(const Mobil& mobil) {
    glPushMatrix();
    glTranslatef(mobil.posX, 8.0f, mobil.posZ);
    glRotatef(mobil.arah, 0, 1, 0);
    
    // Badan
    gambarKotak(0, 0, 0, 20.0f, 8.0f, 40.0f, mobil.r, mobil.g, mobil.b);
    
    // Atap
    gambarKotak(0, 8.0f, -5.0f, 16.0f, 4.0f, 20.0f, mobil.r*0.8f, mobil.g*0.8f, mobil.b*0.8f);
    
    // Kaca
    gambarKotak(0, 8.0f, 15.0f, 14.0f, 4.0f, 0.5f, 0.7f, 0.8f, 0.9f);
    gambarKotak(0, 8.0f, -15.0f, 14.0f, 4.0f, 0.5f, 0.7f, 0.8f, 0.9f);
    
    // Lampu depan
    if (modeMalam) {
        float emisiDepan[] = {0.9f, 0.9f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisiDepan);
    }
    gambarKotak(6.0f, 4.0f, 19.9f, 3.0f, 2.5f, 0.2f, 1.0f, 1.0f, 0.9f);
    gambarKotak(-6.0f, 4.0f, 19.9f, 3.0f, 2.5f, 0.2f, 1.0f, 1.0f, 0.9f);
    
    // Lampu rem
    if (mobil.waktuLampuRem > 0 || mobil.berhentiLampuMerah) {
        float emisiRem[] = {1.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emisiRem);
    }
    gambarKotak(6.0f, 4.0f, -19.9f, 3.0f, 2.5f, 0.2f, 1.0f, 0.0f, 0.0f);
    gambarKotak(-6.0f, 4.0f, -19.9f, 3.0f, 2.5f, 0.2f, 1.0f, 0.0f, 0.0f);
    
    float noEmisi[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmisi);
    
    // Roda
    for (int i = 0; i < 4; i++) {
        float rx = (i % 2 == 0) ? 9.0f : -9.0f;
        float rz = (i < 2) ? 12.0f : -12.0f;
        
        glPushMatrix();
        glTranslatef(rx, -4.0f, rz);
        glRotatef(90.0f, 0, 1, 0);
        glColor3f(0.05f, 0.05f, 0.05f);
        glutSolidTorus(2.0f, 3.5f, 8, 8);
        glPopMatrix();
    }
    
    glPopMatrix();
}

void gambarTaman(float x, float z, float ukuran) {
    // Rumput
    gambarKotak(x, 0.1f, z, ukuran, 0.2f, ukuran, 0.2f, 0.6f, 0.2f);
    
    // Pohon
    for (int i = 0; i < 6; i++) {
        float sudut = (i * 60.0f) * PI / 180.0f;
        float jarak = ukuran * 0.3f;
        float px = x + cos(sudut) * jarak;
        float pz = z + sin(sudut) * jarak;
        
        // Batang
        gambarKotak(px, 5.0f, pz, 2.0f, 10.0f, 2.0f, 0.35f, 0.2f, 0.1f);
        
        // Daun
        for (int lapis = 0; lapis < 2; lapis++) {
            float tinggi = 10.0f + lapis * 6.0f;
            glPushMatrix();
            glTranslatef(px, tinggi, pz);
            glColor3f(0.1f, 0.4f + lapis*0.1f, 0.1f);
            glutSolidSphere(8.0f - lapis*2.0f, 8, 8);
            glPopMatrix();
        }
    }
    
    // Kolam
    gambarKotak(x, 0.05f, z, ukuran*0.2f, 0.1f, ukuran*0.2f, 0.2f, 0.3f, 0.8f);
}

// --- Update Simulasi ---
void updateMobil(float deltaTime) {
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN + LEBAR_TROTOAR*2;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;
    
    for (size_t i = 0; i < daftarMobil.size(); i++) {
        Mobil& mobil = daftarMobil[i];
        
        // Update lampu rem
        if (mobil.waktuLampuRem > 0) {
            mobil.waktuLampuRem -= deltaTime;
        }
        
        // Deteksi lampu lalu lintas
        mobil.diPersimpangan = false;
        mobil.berhentiLampuMerah = false;
        
        for (size_t j = 0; j < daftarLampu.size(); j++) {
            const LampuLaluLintas& lampu = daftarLampu[j];
            float dx = mobil.posX - lampu.posX;
            float dz = mobil.posZ - lampu.posZ;
            float dist = sqrt(dx*dx + dz*dz);
            
            if (dist < 80.0f) {
                mobil.diPersimpangan = true;
                
                if (lampu.status == 0 && dist < 40.0f) {
                    if (!mobil.berhentiLampuMerah) {
                        mobil.waktuLampuRem = 0.5f;
                    }
                    mobil.berhentiLampuMerah = true;
                    break;
                } else if (lampu.status == 1 && dist < 40.0f) {
                    mobil.kecepatan *= 0.7f;
                }
            }
        }
        
        // Bergerak jika tidak berhenti
        if (!mobil.berhentiLampuMerah) {
            float rad = mobil.arah * PI / 180.0f;
            mobil.posX += cos(rad) * mobil.kecepatan * deltaTime * 10.0f;
            mobil.posZ += sin(rad) * mobil.kecepatan * deltaTime * 10.0f;
        }
        
        // Reset jika keluar batas
        float batas = posisiAwal + 400.0f;
        if (abs(mobil.posX) > batas || abs(mobil.posZ) > batas) {
            mobil.rute = rand() % 2;
            if (mobil.rute == 0) {
                mobil.posX = -posisiAwal + randomFloat(0, UKURAN_KOTA * langkah);
                mobil.posZ = -posisiAwal + (rand() % UKURAN_KOTA) * langkah + langkah/2;
                mobil.arah = (rand() % 2) ? 0.0f : 180.0f;
            } else {
                mobil.posX = -posisiAwal + (rand() % UKURAN_KOTA) * langkah + langkah/2;
                mobil.posZ = -posisiAwal + randomFloat(0, UKURAN_KOTA * langkah);
                mobil.arah = (rand() % 2) ? 90.0f : 270.0f;
            }
            mobil.kecepatan = 2.0f + randomFloat(0, 4.0f);
        }
    }
}

void updateLampu(float deltaTime) {
    for (size_t i = 0; i < daftarLampu.size(); i++) {
        LampuLaluLintas& lampu = daftarLampu[i];
        lampu.timer += deltaTime;
        
        if (lampu.timer >= lampu.waktuSiklus) {
            lampu.timer = 0.0f;
            lampu.status = (lampu.status + 1) % 3;
        }
    }
}

// --- Setup Pencahayaan ---
void setupPencahayaan() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Cahaya utama (matahari/bulan)
    float posisiCahaya[4];
    
    if (modeMalam) {
        posisiCahaya[0] = 3000.0f;
        posisiCahaya[1] = 2000.0f;
        posisiCahaya[2] = -3000.0f;
        posisiCahaya[3] = 1.0f;
        
        float ambient[] = {0.1f, 0.1f, 0.15f, 1.0f};
        float diffuse[] = {0.2f, 0.2f, 0.3f, 1.0f};
        float specular[] = {0.1f, 0.1f, 0.2f, 1.0f};
        
        glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glEnable(GL_LIGHT0);
    } else {
        posisiCahaya[0] = 5000.0f;
        posisiCahaya[1] = 5000.0f;
        posisiCahaya[2] = -5000.0f;
        posisiCahaya[3] = 1.0f;
        
        float ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
        float diffuse[] = {1.0f, 1.0f, 0.9f, 1.0f};
        float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        
        glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glEnable(GL_LIGHT0);
    }
}

// --- Tampilan Utama ---
void tampilan() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (modeMalam) {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    } else {
        glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, 1280.0f/720.0f, 1.0f, 10000.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Kamera
    float yawRad = kameraYaw * PI / 180.0f;
    float pitchRad = kameraPitch * PI / 180.0f;
    float lookX = kameraPosisiX + sin(yawRad) * cos(pitchRad);
    float lookY = kameraPosisiY - sin(pitchRad);
    float lookZ = kameraPosisiZ - cos(yawRad) * cos(pitchRad);
    
    gluLookAt(kameraPosisiX, kameraPosisiY, kameraPosisiZ,
              lookX, lookY, lookZ,
              0, 1, 0);
    
    setupPencahayaan();
    
    // Gambar semua elemen
    gambarJalan();
    
    float langkah = UKURAN_BANGUNAN + LEBAR_JALAN + LEBAR_TROTOAR*2;
    float posisiAwal = (UKURAN_KOTA * langkah) / 2.0f;
    
    // Taman
    gambarTaman(-posisiAwal/2, -posisiAwal/2, 200.0f);
    gambarTaman(posisiAwal/2, posisiAwal/2, 200.0f);
    
    // Bangunan
    for (size_t i = 0; i < daftarBangunan.size(); i++) {
        gambarBangunan(daftarBangunan[i]);
    }
    
    // Pos polisi
    gambarPosPolisi(-posisiAwal/3, -posisiAwal/3);
    gambarPosPolisi(posisiAwal/4, posisiAwal/4);
    
    // Lampu jalan
    for (int i = 0; i < UKURAN_KOTA; i++) {
        for (int j = 0; j < UKURAN_KOTA; j++) {
            if ((i + j) % 3 == 0) {
                float x = -posisiAwal + i * langkah + langkah/2;
                float z = -posisiAwal + j * langkah + langkah/2;
                
                gambarLampuJalan(x - langkah/2 + 15, z, 0.0f);
                gambarLampuJalan(x + langkah/2 - 15, z, 180.0f);
                gambarLampuJalan(x, z - langkah/2 + 15, 90.0f);
                gambarLampuJalan(x, z + langkah/2 - 15, 270.0f);
            }
        }
    }
    
    // Lampu lalu lintas
    for (size_t i = 0; i < daftarLampu.size(); i++) {
        gambarLampuLaluLintas(daftarLampu[i]);
    }
    
    // Mobil
    for (size_t i = 0; i < daftarMobil.size(); i++) {
        gambarMobil(daftarMobil[i]);
    }
    
    glutSwapBuffers();
}

// --- Update Loop ---
void perbarui(int value) {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    
    // Update waktu
    waktuGlobal += deltaTime * 0.05f;
    if (waktuGlobal >= 24.0f) waktuGlobal -= 24.0f;
    
    // Mode malam (19:00 - 05:00)
    modeMalam = (waktuGlobal >= 19.0f || waktuGlobal <= 5.0f);
    
    // Input kamera
    float speed = 30.0f * deltaTime;
    float rad = kameraYaw * PI / 180.0f;
    
    if (tombol['w']) {
        kameraPosisiX += sin(rad) * speed;
        kameraPosisiZ -= cos(rad) * speed;
    }
    if (tombol['s']) {
        kameraPosisiX -= sin(rad) * speed;
        kameraPosisiZ += cos(rad) * speed;
    }
    if (tombol['a']) {
        kameraPosisiX -= cos(rad) * speed;
        kameraPosisiZ -= sin(rad) * speed;
    }
    if (tombol['d']) {
        kameraPosisiX += cos(rad) * speed;
        kameraPosisiZ += sin(rad) * speed;
    }
    if (tombol[' ']) kameraPosisiY += speed;
    if (tombol['c']) kameraPosisiY -= speed;
    
    // Update simulasi
    updateLampu(deltaTime);
    updateMobil(deltaTime);
    
    glutPostRedisplay();
    glutTimerFunc(16, perbarui, 0);
}

// --- Input Handler ---
void keyboardTurun(unsigned char key, int x, int y) {
    tombol[key] = true;
    
    if (key == 27) exit(0); // ESC
    
    if (key == 'm' || key == 'M') {
        modeMalam = !modeMalam;
        waktuGlobal = modeMalam ? 22.0f : 12.0f;
    }
    
    if (key == 'n' || key == 'N') {
        modeMalam = true;
        waktuGlobal = 22.0f;
    }
    
    if (key == 'd' || key == 'D') {
        modeMalam = false;
        waktuGlobal = 12.0f;
    }
}

void keyboardLepas(unsigned char key, int x, int y) {
    tombol[key] = false;
}

void mouseMove(int x, int y) {
    if (mouseAktif) {
        kameraYaw += (x - mouseTerakhirX) * 0.3f;
        kameraPitch += (y - mouseTerakhirY) * 0.3f;
        
        if (kameraPitch > 89.0f) kameraPitch = 89.0f;
        if (kameraPitch < -89.0f) kameraPitch = -89.0f;
        
        mouseTerakhirX = x;
        mouseTerakhirY = y;
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseAktif = !mouseAktif;
        mouseTerakhirX = x;
        mouseTerakhirY = y;
        
        if (mouseAktif) {
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutSetCursor(GLUT_CURSOR_INHERIT);
        }
    }
}

// --- Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Kota Realistis - Dev-C++");
    
    // Inisialisasi OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    // Inisialisasi kota
    inisialisasiKota();
    
    // Callback
    glutDisplayFunc(tampilan);
    glutKeyboardFunc(keyboardTurun);
    glutKeyboardUpFunc(keyboardLepas);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutTimerFunc(0, perbarui, 0);
    
    // Informasi
    printf("=== KOTA REALISTIS ===\n");
    printf("Kontrol:\n");
    printf("WASD - Gerak kamera\n");
    printf("Spasi/C - Naik/turun\n");
    printf("Klik kiri - Kunci mouse\n");
    printf("M - Toggle siang/malam\n");
    printf("N - Mode malam\n");
    printf("D - Mode siang\n");
    printf("ESC - Keluar\n");
    
    glutMainLoop();
    return 0;
}
