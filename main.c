#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "qrcodegen.h"
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_ITEMS 20

typedef struct {
    char name[50];
    int price;
} MenuItem;

typedef struct {
    char name[50];
    int quantity;
} CartItem;

void generate_qr_png(const uint8_t qrcode[], const char *filename) {
    int size = qrcodegen_getSize(qrcode);
    int scale = 10;
    int margin = 4;
    int img_size = (size + 2 * margin) * scale;

    unsigned char *img = (unsigned char *)malloc(img_size * img_size * 3);
    if (!img) {
        fprintf(stderr, "Gagal mengalokasikan memori untuk gambar.\n");
        return;
    }

    for (int y = 0; y < img_size; y++) {
        for (int x = 0; x < img_size; x++) {
            img[(y * img_size + x) * 3 + 0] = 255;
            img[(y * img_size + x) * 3 + 1] = 255;
            img[(y * img_size + x) * 3 + 2] = 255;
        }
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int color = qrcodegen_getModule(qrcode, x, y) ? 0 : 255;
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    int px = (margin + x) * scale + dx;
                    int py = (margin + y) * scale + dy;
                    img[(py * img_size + px) * 3 + 0] = color;
                    img[(py * img_size + px) * 3 + 1] = color;
                    img[(py * img_size + px) * 3 + 2] = color;
                }
            }
        }
    }

    if (stbi_write_png(filename, img_size, img_size, 3, img, img_size * 3)) {
    } else {
        fprintf(stderr, "Gagal menyimpan gambar QR.\n");
    }

    free(img);
}

void generateQRCode(const char *data) {
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText(data, tempBuffer, qrcode, qrcodegen_Ecc_LOW,
                                   qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);

    if (ok) {
        int size = qrcodegen_getSize(qrcode);
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                fputs(qrcodegen_getModule(qrcode, x, y) ? "##" : "  ", stdout);
            }
            fputs("\n", stdout);
        }
    } else {
        printf("Gagal menghasilkan QR Code. Coba dengan data yang lebih pendek atau versi lebih besar.\n");
    }
}

void displayMenuAndAddToCart(MenuItem menu[], CartItem cart[], int *cartCount, int menuCount) {
    int choice, quantity;

    system("cls");
    printf("\n===================== PROLOG KOPI =====================\n\n");
    printf("+%-4s+%-31s+%-11s+\n", "-----", "--------------------------------", "------------");
    printf("| %-3s | %-30s | %-10s |\n", "No", "             Menu", "Harga (Rp)");
    printf("+%-4s+%-31s+%-11s+\n", "-----", "--------------------------------", "------------");
    for (int i = 0; i < menuCount; i++) {
        printf("| %-3d | %-30s | Rp%3d.%03d  |\n", i + 1, menu[i].name, menu[i].price, 0);
    }
    printf("+%-4s+%-31s+%-11s+\n", "-----", "--------------------------------", "------------");
    printf("\n===================== PROLOG KOPI =====================\n");

    while (1) {
        printf("Masukkan Pilihan Menu (0 Untuk Keluar): ");
        scanf("%d", &choice);

        if (choice == 0) {
            printf("\nTerima kasih telah menggunakan layanan kami. Sampai Jumpa!\n");
            exit(0);
        }

        if (choice < 1 || choice > menuCount) {
            printf("Menu tidak valid! Silakan pilih nomor menu yang benar.\n");
        } else {
            break;
        }
    }

    printf("Masukkan Jumlah Pesanan: ");
    scanf("%d", &quantity);

    int totalHarga = menu[choice - 1].price * quantity;
    system("cls");
    printf("======================== PROLOG KOPI ========================\n");
    printf("\n+--------------------+-----------+------------+------------+\n");
    printf("| %-18s | %-9s | %-10s | %-10s |\n", "       Menu", "Harga", "Quantity", "Total");
    printf("+--------------------+-----------+------------+------------+\n");
    printf("| %-18s | Rp%3d.%03d | %-10d | Rp%3d.%03d  |\n", menu[choice - 1].name, menu[choice - 1].price, 0, quantity, totalHarga, 0);
    printf("+--------------------+-----------+------------+------------+\n");
    printf("\nPesanan Berhasil Ditambahkan!\n");
    printf("\n======================== PROLOG KOPI ========================\n");

    int found = 0;
    for (int i = 0; i < *cartCount; i++) {
        if (strcmp(cart[i].name, menu[choice - 1].name) == 0) {
            cart[i].quantity += quantity;
            found = 1;
            break;
        }
    }

    if (!found) {
        strcpy(cart[*cartCount].name, menu[choice - 1].name);
        cart[*cartCount].quantity = quantity;
        (*cartCount)++;
    }

    int nextChoice;
    printf("1. Tambahkan Pesanan Lain\n");
    printf("2. Cek Keranjang\n");
    printf("Pilih opsi: ");
    scanf("%d", &nextChoice);

    if (nextChoice == 1) {
        displayMenuAndAddToCart(menu, cart, cartCount, menuCount);
    } else if (nextChoice == 2) {
        system("cls");
        displayCart(cart, *cartCount, menu, menuCount);
    } else if (nextChoice == 0) {
        printf("\nTerima kasih telah menggunakan layanan kami. Sampai Jumpa!\n");
        exit(0);
    } else {
        printf("Pilihan tidak valid! Kembali ke menu utama.\n");
    }
}

void displayCart(CartItem cart[], int cartCount, MenuItem menu[], int menuCount) {
    int totalKeranjang = 0;

    printf("======================== PROLOG KOPI ========================\n");
    printf("\n+--------------------+-----------+------------+------------+\n");
    printf("| %-18s | %-9s | %-10s | %-10s |\n", "       Menu", "Harga", "Quantity", "Total");
    printf("+--------------------+-----------+------------+------------+\n");
    if (cartCount == 0) {
        printf("Keranjang Anda kosong.\n");
    } else {
        for (int i = 0; i < cartCount; i++) {
            int price = 0;
            for (int j = 0; j < menuCount; j++) {
                if (strcmp(cart[i].name, menu[j].name) == 0) {
                    price = menu[j].price;
                    break;
                }
            }
            int totalItem = cart[i].quantity * price;
            totalKeranjang += totalItem;

            printf("| %-18s | Rp%3d.%03d | %-10d | Rp%3d.%03d  |\n", cart[i].name, price, 0, cart[i].quantity, totalItem, 0);
        }
        printf("+--------------------+-----------+------------+------------+\n");
        printf("| %-18s  %-24s  Rp%3d.%03d  |\n", "Total Pesanan", "", totalKeranjang, 0);
        printf("+--------------------+-----------+------------+------------+\n");
        printf("\n======================== PROLOG KOPI ========================\n");
    }

    int choice;
    printf("1. Tambah Pesanan Lain\n");
    printf("2. Lanjut Ke Pembayaran\n");
    printf("Pilih opsi: ");
    scanf("%d", &choice);

    if (choice == 1) {
        displayMenuAndAddToCart(menu, cart, &cartCount, menuCount);
    } else if (choice == 2) {
        int paymentChoice;
        system("cls");
        printf("\nPilih Metode Pembayaran:\n");
        printf("1. Cash\n");
        printf("2. QRIS\n");
        printf("Pilih opsi: ");
        scanf("%d", &paymentChoice);

        if (paymentChoice == 1) {
            system("cls");
            printf("======================== PROLOG KOPI ========================\n");
            printf("\n+--------------------+-----------+------------+------------+\n");
            printf("| %-18s | %-9s | %-10s | %-10s |\n", "       Menu", "Harga", "Quantity", "Total");
            printf("+--------------------+-----------+------------+------------+\n");

            for (int i = 0; i < cartCount; i++) {
                int price = 0;
                for (int j = 0; j < menuCount; j++) {
                        if (strcmp(cart[i].name, menu[j].name) == 0) {
                            price = menu[j].price;
                break;
                }
            }
            int totalItem = cart[i].quantity * price;
            printf("| %-18s | Rp%3d.%03d | %-10d | Rp%3d.%03d  |\n", cart[i].name, price, 0, cart[i].quantity, totalItem, 0);
        }
        printf("+--------------------+-----------+------------+------------+\n");
        printf("| %-18s  %-24s  Rp%3d.%03d  |\n", "Total Pesanan", "", totalKeranjang, 0);
        printf("+--------------------+-----------+------------+------------+\n");
        printf("\n======================== PROLOG KOPI ========================\n");

        int payment;
        printf("Masukkan jumlah uang yang dibayar: Rp");
        scanf("%d", &payment);

        displayInvoice(cart, cartCount, menu, menuCount, totalKeranjang, payment);
            } else if (paymentChoice == 2) {
                displayInvoice(cart, cartCount, menu, menuCount, totalKeranjang);
                char qrData[50];
                sprintf(qrData, "Total Pembayaran: Rp%d.000", totalKeranjang);
                uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
                uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

                bool success = qrcodegen_encodeText(qrData, tempBuffer, qrcode,
                                                    qrcodegen_Ecc_LOW,
                                                    qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                                                    qrcodegen_Mask_AUTO, true);

            if (success) {
                generate_qr_png(qrcode, "QRIS_Pembayaran.png");
#ifdef _WIN32
                system("start QRIS_Pembayaran.png");
#endif
            } else {
                printf("Gagal menghasilkan QR Code. Coba dengan data yang lebih pendek atau versi lebih besar.\n");
            }
        } else {
            printf("Pilihan tidak valid! Kembali ke menu utama.\n");
        }
    }
}

void displayInvoice(CartItem cart[], int cartCount, MenuItem menu[], int menuCount, int total, int payment, int isQris) {
    int totalHarga = 0;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    system("cls");
    printf("\n==================== INVOICE =====================\n");
    printf("%04d-%02d-%02d %-27s %-15s\n%02d:%02d:%02d\n",
           tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday, "", "Prolog Kopi",
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    printf("--------------------------------------------------");

    for (int i = 0; i < cartCount; i++) {
        for (int j = 0; j < menuCount; j++) {
            if (strcmp(cart[i].name, menu[j].name) == 0) {
                int priceInRupiah = menu[j].price * 1000;
                int subtotal = cart[i].quantity * priceInRupiah;
                printf("\n%s\n", cart[i].name);
                printf("%d x Rp%3d.%03d %-25s Rp%3d.%03d", cart[i].quantity, priceInRupiah / 1000, priceInRupiah % 1000, "",
                       subtotal / 1000, subtotal % 1000);
                totalHarga += subtotal;
                break;
            }
        }
    }

    printf("\n--------------------------------------------------");
    printf("\nTotal %-33s Rp%3d.%03d\n", "", totalHarga / 1000, totalHarga % 1000);

    if (!isQris) {
        if (payment >= totalHarga) {
            int kembalian = payment - totalHarga;
            printf("Bayar (Cash) %-26s Rp%3d.%03d\n", "", payment / 1000, payment % 1000);
            if (kembalian > 0) {
                printf("Kembali %-31s Rp%3d.%03d\n", "", kembalian / 1000, kembalian % 1000);
            } else {
                printf("Kembali %-31s Rp%3d.%03d\n", "", kembalian / 1000, kembalian % 1000);
            }
        }

    printf("==================== INVOICE =====================\n");
    printf("\nTerima kasih telah menggunakan layanan kami. Sampai Jumpa!\n");
    }
}


int main() {
    MenuItem menu[MAX_ITEMS] = {
        {"Signature Dialog", 12},
        {"Signature Monolog", 9},
        {"Salt Caramel", 15},
        {"Ice Coffee Banana", 15},
        {"Ice Coffee Almond", 15},
        {"Ice Coffee Pandan", 15},
        {"Ice Coffee Berry", 15},
        {"Irish Coffee", 15},
        {"Ice Coffee Nutella", 17},
        {"Black Forest", 15},
        {"Matcha Latte", 17},
        {"Violent Tea", 15},
        {"Creatella", 15},
        {"Tea on Berry's", 15},
        {"Lemon Tea", 10},
        {"Fresh Tea", 5}
    };

    CartItem cart[MAX_ITEMS];
    int cartCount = 0;

    displayMenuAndAddToCart(menu, cart, &cartCount, 16);

    return 0;
}
