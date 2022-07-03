#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/**
    TUGAS MEMBUAT PROGRAM APROKSIMASI FUNGSI POLINOMIAL, NILAI TURUNAN, DAN NILAI INTEGRAL DENGAN METODE NUMERIK
    Nama    : Raflizal Fikrar Odriansyah
    NIM     : 221910812
    Kelas   : 3SD1
**/

/*
Keterangan:
    Fungsi PolynomialNewton()
    Fungsi ini digunakan untuk membangun polinomial Newton ketika diketahui
    nilai koefisien polinomial (a), centroid (c), dan derajat polinomial x.
    Bentuk polinomial Newton: y = a0 + a1(x - c1) + a2(x - c1)(x - c2) + a3(x - c1)(x - c2)(x - c3) + ...
*/

float PolynomialNewton(float a[], float c[], float x, int size_a){
    if (size_a == 1)
        return a[0];

    float temp = 1, result = a[0];

    for (int i = 0; i < size_a - 1; i++){
        temp *= x - c[i];
        result += a[i + 1] * temp;
    }

    return result;
}

/*
Keterangan:
    Fungsi PolynomialCollocation()
    Fungsi ini digunakan untuk membangun suatu polinomial kolokasi yang diketahui titik-titik kolokasinya.
    points adalah array 1 dimensi dimana 1 titik kolokasi (x dan y) menempati 2 elemen pada array.
*/

float **PolynomialCollocation(float points[], int n, int num_ref){
    static float a[128];
    static float c[128];
    if (n < 2){
        printf("!ERROR! Titik kolokasi harus bernilai lebih dari 1");
        return 0;
    }

    for (int i = 0; i < n; i++){
        c[i] = points[i*2];
    }

    a[0] = points[1];

    for (int i = 1; i < num_ref; i++){
        float temp = 1;

        for (int j = 0; j < i; j++){
            temp *= points[i*2] - c[j];
        }
        a[i] = (points[i*2 + 1] - PolynomialNewton(a, c, points[i*2], i + 1)) / temp;
    }

    static float *p[] = {a, c};
    return p;
}

/*
Keterangan:
    Fungsi Derivative()
    Fungsi ini digunakan untuk mencari nilai turunan fungsi pada titik x tertentu
*/

float Derivative(float a[], float c[], float x, float h, int n){
    float result;
    result = (PolynomialNewton(a, c, x + h, n) - PolynomialNewton(a, c, x, n)) / h;
    return result;
}

/*
Keterangan:
    Fungsi Integral()
    Fungsi yang digunakan untuk mencari nilai integral dari sebuah fungsi menggunakan
    metode integral Jumlah Riemann.
*/

float Integral(float a[], float c[], float b1, float b2, float h, int n){
    float result = 0;
    int iter = (b2 - b1) / h;

    if (iter > 1000){
        printf("\nError! Batas iterasi sudah mencapai maksimum 1000 kali!");
        return 0;
    }

    for (int i = 0; i < iter; i++){
        result += h * PolynomialNewton(a, c, b1 + i*h / 2, n);
    }

    return result;
}

/*
Keterangan:
    Fungsi IndexMinArray
    Fungsi ini digunakan ntuk mencari indeks dari elemen terkecil pada sebuah array.
*/

int IndexMinArray(float array[], int n, int idx){
    int minIndex = idx;
    for (int i = idx; i < n; i++){
        if (array[i] < array[minIndex])
            minIndex = i;
    }
    return minIndex;
}

/*
Keterangan:
    Fungsi sort()
    Fungsi ini digunakan untuk mengurutkan array yang berisi titik-titik kolokasi secara ascending;
    Isian array 1 dimensi meliputi titik - titik kolokasi seperti: [x1,y1,x2,y2,...,xn,yn]
*/

float *SortingArray(float points[], int n){

    float x[128], temp;
    for (int i = 0; i < n; i++){
        x[i] = points[i*2];
    }

    int minIndex_X;

    for (int i = 0; i < n; i++){
        minIndex_X = IndexMinArray(x, n, i);
        temp = x[i];
        x[i] = x[minIndex_X];
        x[minIndex_X] = temp;
        temp = points[i * 2];
        points[i * 2] = points[minIndex_X*2];
        points[minIndex_X*2] = temp;
        temp = points[i * 2 + 1];
        points[i * 2 + 1] = points[minIndex_X*2 + 1];
        points[minIndex_X*2 + 1] = temp;
    }

    return points;
}

/*
Keterangan:
    Fungsi x_a_Position()
    Fungsi ini digunakan untuk mencari posisi index dari nilai xa di antara titik - titik kolokasi yang diberikan
    dengan asumsi array berisi titik-titik kolokasi sudah diurutkan.
*/

int x_a_Position(float points[], float x_a, int n){

    int position = 0;
    float precision = 0.001;

    for (int i = 0; i < n; i++){
        if ((x_a + precision) >= points[i*2]){
            position = i + 1;
        }
    }

    return position;
}

/*
Keterangan:
    Fungsi GetValueY()
    Fungsi ini digunakan untuk memperoleh nilai fungsi y dari titik-titik kolokasi yang diberikan.
    Parameter global digunakan untuk memberikan penanda apabila bahwa nilai x titik kolokasi tidak ditemukan
    sehingga tidak dapat menggunakan formula.
*/

float GetValueY(float points[], float x, int n, int *global){
    float precision = 0.001;
    for (int i = 0; i < n; i++){
        if (fabsf(points[i*2] - x) < precision)
            return points[i*2 + 1];
    }

    printf("\nPeringatan! Nilai p(%.3f) tidak ditemukan pada titik kolokasi yang diberikan", x);
    *global = -1;
    return -9999;
}

/*
Keterangan:
    Fungsi getDerivativePoints()
    Fungsi ini digunakan untuk mendapatkan titik-titik yang dibutuhkan sesuai dengan metode aproksimasi yang digunakan.
    Outputnya berupa array dari titik-titik yang akan digunakan dalam formula aproksimasi turunan numerik.

    parameter kodeFormula berupa nilai kode formula yang menunjukkan
    1 = formula selisih terpusat (selisih tengah)
    2 = formula 3 titik dimana x0 berada di ujung kiri urutan titik-titik kolokasi (x0 = x0 - h)
    3 = formula 5 titik dimana x0 berada di ujung kiri urutan titik-titik kolokasi (x0 = x0 - 2h)
    4 = formula 5 titik dimana x0 berada di tengah urutan titik-titik kolokasi (x0 = x0)
    0 = tidak dapat menggunakan formula aproksimasi turunan, sehingga perlu membangun polinomial kolokasi terlebih dahulu
*/

float *getDerivativePoints(float points[], float x, int n, float h, int num_ref, int *kodeFormula, int *global){
    static float collocation[20];
    int x_position = x_a_Position(points, x, n);

    if (num_ref != 2 && num_ref != 3 && num_ref != 5){
        printf("Peringatan! Titik referensi yang bisa digunakan hanya sebanyak 2, 3, atau 5 titik saja.");
    }
    else if ((num_ref != 3 && num_ref!= 5) && (x_position == 0 || x_position >= n)){
        printf("Peringatan! Titik yang dicari (Xo = %.3f) kurang dari titik kolokasi minimum (Xo < %.3f) atau lebih dari sama dengan dari titik kolokasi maksimum (xo > %.3f)", x, points[0], points[2*(n - 1)]);
    }

    // Jika banyak titik referensi yang digunakan yaitu sejumlah 2 atau 3 jika ksi berada di antara xo-h dan xo + h,
    // maka gunakan formula selisih terpusat (selisih tengah)
    else if (num_ref == 2 || (num_ref == 3 && (x_position > n - 2))){
        float y0 = GetValueY(points, x, n, global);
        if (*global != -1){
            *kodeFormula = 1;
            collocation[0] = x - h;
            collocation[1] = GetValueY(points, x - h, n, global);
            collocation[2] = x + h;
            collocation[3] = GetValueY(points, x + h, n, global);
        }
    }

    // Jika banyak titik referensi yang digunakan adalah 3 dan titik ksi berada di antara xo dan xo + 2h
    else if (num_ref == 3){
        float y0 = GetValueY(points, x, n, global);
        if (*global != -1){
            *kodeFormula = 2;
            collocation[0] = x;
            collocation[1] = y0;
            collocation[2] = x + h;
            collocation[3] = GetValueY(points, x + h, n, global);
            collocation[4] = x + 2*h;
            collocation[5] = GetValueY(points, x + 2*h, n, global);
        }
    }

    // Jika banyak titik referensi yang digunakan sejumlah 5, tetapi jumlah titik kolokasinya kurang dari 5
    else if (num_ref == 5 && x_position > n - 2){
        printf("Peringatan! Hanya ada sejumlah %d titik kolokasi, dimana Xo terdapat pada indeks ke-%d sedangkan formula aproksimasi membutuhkan 5 titik referensi", n, x_position);
    }

    // Jika banyak titik referensi yang digunakan sejumlah 5 dan nilai ksi berada pada xo dan xo + 4h
    else if (num_ref == 5 && x_position < 2){
        float y0 = GetValueY(points, x, n, global);
        if (*global != -1){
            *kodeFormula = 3;
            collocation[0] = x;
            collocation[1] = y0;
            collocation[2] = x + h;
            collocation[3] = GetValueY(points, x + h, n, global);
            collocation[4] = x + 2*h;
            collocation[5] = GetValueY(points, x + 2*h, n, global);
            collocation[6] = x + 3*h;
            collocation[7] = GetValueY(points, x + 3*h, n, global);
            collocation[8] = x + 4*h;
            collocation[9] = GetValueY(points, x + 4*h, n, global);
        }
    }

    // Jika banyak titik referensi yang digunakan sejumlah 5 dan nilai ksi berada pada x0 - 2h dan x0 + 2h
    else{
        float yo = GetValueY(points, x, n, global);
        if (*global != -1){
            *kodeFormula = 4;
            collocation[0] = x - 2*h;
            collocation[1] = GetValueY(points, x - 2 * h, n, global);
            collocation[2] = x - h;
            collocation[3] = GetValueY(points, x - h, n, global);
            collocation[4] = x + h;
            collocation[5] = GetValueY(points, x + h, n, global);
            collocation[6] = x + 2*h;
            collocation[7] = GetValueY(points, x + 2 * h, n, global);
        }
    }

    return collocation;
}

// Formula Selisih Terpusat (Selisih Tengah)
float centralDifferenceApproximation(float collocation[], float h){
    float result;
    result = (collocation[3] - collocation[1]) / (2*h);
    return result;
}

// Formula Tiga Titik ketika x berada pada ujung kiri (x0-h)
float threePointsDifferenceApproximation(float collocation[], float h){
    float result;
    result = ((-3)*collocation[1] + 4*collocation[3] - collocation[5]) / (2 * h);
    return result;
}

// Formula lima titik ketika titik x berada pada ujung kiri (x0-2h)
float fivePointsLeftEndApproximation(float collocation[], float h){
    float result;
    result = ((-25)*collocation[1] + 48*collocation[3] - 36*collocation[5] + 16*collocation[7] - 3*collocation[9]) / (12*h);
    return result;
}

// Formula lima titik ketika titik x berada pada titik tengah (x0)
float fivePointsCentralApproximation(float collocation[], float h){
    float result;
    result = (collocation[1] - 8*collocation[3] + 8*collocation[5] - collocation[7]) / (12*h);
    return result;
}

/*
Keterangan:
    Fungsi getIntegrationPoints()
    Fungsi yang digunakan untuk memperoleh titik-titik yang dibutuhkan sesuai dengan metode aproksimasi integral yang digunakan
*/

float *getIntegralPoints(float points[], int n, float h, int *global){
    *global = 0;
    static float collocation[6];
    float x = (points[0] + points[2*(n - 1)]) / 2;
    int x_position = x_a_Position(points, x, n);

    float y0 = GetValueY(points, x, n, global);
    if (*global != -1){
        collocation[0] = x - h;
        collocation[1] = GetValueY(points, x - h, n, global);
        collocation[2] = x;
        collocation[3] = y0;
        collocation[4] = x + h;
        collocation[5] = GetValueY(points, x + h, n, global);
    }

    return collocation;
}

/*
Keterangan:
    Fungsi simpsonMethodApproximation()
    Fungsi yang digunakan untuk menghitung nilai integral aproksimasi dengan metode simpsons secara numerik
*/
float simpsonMethodApproximation(float collocation[]){
    float result;
    result = (collocation[1] + 4*collocation[3] + collocation[5])*(collocation[4] - collocation[0]) / 6;
    return result;
}

/*
Keterangan:
    Fungsi midpointMethodApproximation()
    Fungsi yang digunakan untuk menghitung nilai integral aproksimasi dengan metode midpoint atau titik tengah secara numerik
*/
float midpointMethodApproximation(float collocation[]){
    float result;
    result = (collocation[4] - collocation[0])*collocation[3];
    return result;
}

/*
Keterangan:
    Fungsi trapezoidalMethodApproximation()
    Fungsi yang digunakan untuk menghitung nilai integral aproksimasi dengan metode trapesium secara numerik
*/
float trapezoidalMethodApproximation(float collocation[]){
    float result;
    result = ((collocation[4] - collocation[0])/2)*(collocation[1] + collocation[5]);
    return result;
}

// Fungsi utama untuk menampilkan menu program dan menjalankan program
int main(){
    int global = 0;                 // Kode keberhasilan penggunaan formula aproksimasi
    int choose = -1;                // Nomor pilihan menu
    int kodeFormula = 0;            // Kode formula turunan yang digunakan
    char freeze[1];                  // Untuk mem-freeze layar setelah menampilkan output
    int num_point;                  // Jumlah titik-titik kolokasi yang diinput
    float points[128];              // Array 1 dimensi berisi pasangan titik-titik kolokasi x dan f(x)

    printf("\nPROGRAM MENCARI APROKSIMASI FUNGSI POLINOMIAL, NILAI TURUNAN, DAN NILAI INTEGRAL DENGAN METODE NUMERIK\n");
    printf("======================================================================================================\n\n");
    printf("Masukkan jumlah titik-titik kolokasi (minimal 2 dan maksimal 64) : ");
    scanf("%d", &num_point);
    for (int i = 0; i < 2*num_point; i++){
        printf("Masukkan titik-titik kolokasi [%d, %d] : ", (i / 2) + 1, (i % 2) + 1);
        scanf("%f", &points[i]);
    }

    while (choose != 0){
        system("cls");
        printf("\nTitik-titik kolokasi : ");

        for (int i = 0; i < num_point; i++){
            printf("(%.3f, %.3f); ", points[i*2], points[i*2 + 1]);
        }

        printf("\n\n\t\tPilihan Menu Metode Aproksimasi\n");
        printf("=================================================================\n");
        printf("1. Aproksimasi Fungsi Polinomial dengan Metode Numerik\n");
        printf("2. Aproksimasi Nilai Turunan Fungsi dengan Metode Numerik\n");
        printf("3. Aproksimasi Nilai Integral Fungsi dengan Metode Numerik\n");
        printf("4. Keluar Program\n\n");
        printf("Masukkan pilihan: ");
        scanf("%d", &choose);

        int num_ref;
        float x, h;

        if (choose == 1){
            printf("\n\t\t  Polinomial Kolokasi\n");
            printf("=================================================================\n");
            printf("Masukkan x yang ingin dicari : ");
            scanf("%f", &x);
            printf("Masukkan jumlah titik referensi yang ingin digunakan : ");
            scanf("%d", &num_ref);
            float **result = PolynomialCollocation(points, num_point, num_ref);
            printf("\n\t\tHasil Aproksimasi Polinomial Kolokasi\n=================================================================\n");
            printf("Titik referensi yang digunakan : %d", num_ref);
            printf("\n\nPolinomial kolokasi yang terbentuk:\nP(x) = ");

            for (int i = 0; i < num_ref; i++){
                printf("(%.3f)", result[0][i]);

                for (int j = 0; j < i; j++){
                    printf("(x - %.3f))", result[1][j]);
                }
                if (i != num_ref - 1)
                    printf(" + ");
            }
            printf("\n\nNilai P(%.3f) adalah %.3f", x, PolynomialNewton(result[0], result[1], x, num_point));
        }
        else if (choose == 2){
            printf("\n\t\tAproksimasi Turunan Numerik\n");
            printf("=================================================================\n");
            printf("Masukkan titik x yang ingin diaproksimasi : ");
            scanf("%f", &x);
            printf("Masukkan jumlah titik referensi yang ingin digunakan : ");
            scanf("%d", &num_ref);
            printf("Masukkan nilai h : ");
            scanf("%f", &h);
            float *sortedPoints = SortingArray(points, num_point);
            float *collocation = getDerivativePoints(sortedPoints, x, num_point, h, num_ref, &kodeFormula, &global);
            if (global == -1 || kodeFormula == 0){
                printf("\nPeringatan! Formula aproksimasi tidak dapat digunakan sehingga perlu membangun polinomial kolokasinya terlebih dahulu");
                float **result = PolynomialCollocation(sortedPoints, num_point, num_point);
                printf("\nHasil aproksimasi : P'(%.3f) = %.3f", x, Derivative(result[0], result[1], x, h, num_point));
            }
            else{
                float result;
                if (kodeFormula == 1){
                    printf("\n\t\tGunakan formula titik tengah\n");
                    printf("-----------------------------------------------------------------");
                    result = centralDifferenceApproximation(collocation, h);
                }
                else if (kodeFormula == 2){
                    printf("\n\t\tGunakan formula 3 titik\n");
                    printf("-----------------------------------------------------------------");
                    result = threePointsDifferenceApproximation(collocation, h);
                }
                else if (kodeFormula == 3){
                    printf("\n\tGunakan formula 5 titik dengan titik x0 berada di tepi kiri\n");
                    printf("-----------------------------------------------------------------");
                    result = fivePointsLeftEndApproximation(collocation, h);
                }
                else if (kodeFormula == 4){
                    printf("\n\tGunakan formula 5 titik dengan titik x0 berada di tengah\n");
                    printf("-----------------------------------------------------------------");
                    result = fivePointsCentralApproximation(collocation, h);
                }
                printf("\nNilai Turunan Aproksimasi: P'(%.3f) = %.3f", x, result);
            }
        }
        else if (choose == 3){
            printf("\n\t\tAproksimasi Integral Numerik Jumlah Riemann\n");
            printf("=================================================================");
            printf("\nMasukkan nilai h : ");
            scanf("%f", &h);
            float *sortedPoints = SortingArray(points, num_point);
            float *collocation = getIntegralPoints(sortedPoints, num_point, h, &global);
            if (global == -1){
                printf("\nPeringatan! Formula aproksimasi tidak dapat digunakan sehingga perlu membangun polinomial kolokasinya terlebih dahulu");
                float **result = PolynomialCollocation(sortedPoints, num_point, num_point);
                printf("\nHasil aproksimasi dari polinomial kolokasi: %.3f", Integral(result[0], result[1], sortedPoints[0], sortedPoints[2*(num_point-1)], h, num_point));
            }
            else{
                printf("\n\t\tHasil Aproksimasi Integral Numerik\n");
                printf("=================================================================");
                printf("\nMetode MidPoint       : %.3f", midpointMethodApproximation(collocation));
                printf("\nMetode Trapezoidal    : %.3f", trapezoidalMethodApproximation(collocation));
                printf("\nMetode Simpsons       : %.3f", simpsonMethodApproximation(collocation));
            }
        }
        printf("\n\nKetik apapun untuk melanjutkannya: ");
        scanf("%s", freeze);
    }

    return 0;
}
