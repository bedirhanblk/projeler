#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <typeinfo>  // typeid için eklendi

using namespace std;

// Temel Kitap sýnýfý
class Kitap {
protected:
    string ISBN;
    string baslik;
    string yazar;
    int yayinYili;
    bool oduncAlindi;

public:
    Kitap(string isbn, string title, string author, int year)
        : ISBN(isbn), baslik(title), yazar(author), yayinYili(year), oduncAlindi(false) {}

    // Getter fonksiyonlarý
    string getISBN() const { return ISBN; }
    string getBaslik() const { return baslik; }
    string getYazar() const { return yazar; }
    int getYayinYili() const { return yayinYili; }
    bool isOduncAlindi() const { return oduncAlindi; }

    // Setter fonksiyonlarý
    void setOduncAlindi(bool durum) { oduncAlindi = durum; }

    // Operatör yükleme (== operatörü ISBN'e göre karþýlaþtýrma)
    bool operator==(const Kitap& other) const {
        return this->ISBN == other.ISBN;
    }

    // Sanal fonksiyon (polymorphism için)
    virtual void bilgileriGoster() const {
        cout << "ISBN: " << ISBN << "\nBaslik: " << baslik
             << "\nYazar: " << yazar << "\nYayin Yili: " << yayinYili
             << "\nDurum: " << (oduncAlindi ? "Odunc Alindi" : "Rafta") << endl;
    }

    // Dosyaya yazmak için sanal fonksiyon
    virtual string dosyaFormatinda() const {
        return ISBN + "," + baslik + "," + yazar + "," + to_string(yayinYili) + "," + (oduncAlindi ? "1" : "0");
    }
};

// Dergi sýnýfý (Kitap sýnýfýndan türetilmiþ)
class Dergi : public Kitap {
private:
    int sayiNo;
    string yayinAyi;

public:
    Dergi(string isbn, string title, string author, int year, int issue, string month)
        : Kitap(isbn, title, author, year), sayiNo(issue), yayinAyi(month) {}

    void bilgileriGoster() const override {
        Kitap::bilgileriGoster();
        cout << "Sayi No: " << sayiNo << "\nYayin Ayi: " << yayinAyi << endl;
    }

    string dosyaFormatinda() const override {
        return Kitap::dosyaFormatinda() + "," + to_string(sayiNo) + "," + yayinAyi + ",dergi";
    }
};

// Üye sýnýfý
class Uye {
private:
    string uyeNo;
    string ad;
    string soyad;
    vector<string> oduncKitaplar;

public:
    Uye(string no, string name, string surname)
        : uyeNo(no), ad(name), soyad(surname) {}

    // Getter fonksiyonlarý
    string getUyeNo() const { return uyeNo; }
    string getAd() const { return ad; }
    string getSoyad() const { return soyad; }
    const vector<string>& getOduncKitaplar() const { return oduncKitaplar; }

    // Kitap ödünç alma/iade etme
    void kitapOduncAl(string ISBN) {
        oduncKitaplar.push_back(ISBN);
    }

    void kitapIadeEt(string ISBN) {
        oduncKitaplar.erase(remove(oduncKitaplar.begin(), oduncKitaplar.end(), ISBN), oduncKitaplar.end());
    }

    // Operatör yükleme (<< operatörü için)
    friend ostream& operator<<(ostream& os, const Uye& uye) {
        os << "Uye No: " << uye.uyeNo << "\nAd: " << uye.ad << "\nSoyad: " << uye.soyad;
        return os;
    }

    // Dosyaya yazmak için fonksiyon
    string dosyaFormatinda() const {
        string result = uyeNo + "," + ad + "," + soyad;
        for (const auto& isbn : oduncKitaplar) {
            result += "," + isbn;
        }
        return result;
    }
};

// Kütüphane yönetim sýnýfý
class Kutuphane {
private:
    vector<Kitap*> kitaplar;
    vector<Uye*> uyeler;

    // Þablon fonksiyon - hem kitaplar hem üyeler için arama yapabilir
    template<typename T>
    T* ara(const vector<T*>& liste, const string& id) {
        for (auto item : liste) {
            if (item->getISBN() == id) {
                return item;
            }
        }
        return nullptr;
    }

    // Uye aramasý için özelleþtirilmiþ fonksiyon
    Uye* araUye(const vector<Uye*>& liste, const string& id) {
        for (auto uye : liste) {
            if (uye->getUyeNo() == id) {
                return uye;
            }
        }
        return nullptr;
    }

public:
    ~Kutuphane() {
        for (auto kitap : kitaplar) delete kitap;
        for (auto uye : uyeler) delete uye;
    }

    // Kitap ekleme
    void kitapEkle(Kitap* kitap) {
        kitaplar.push_back(kitap);
    }

    // Üye ekleme
    void uyeEkle(Uye* uye) {
        uyeler.push_back(uye);
    }

    // Kitap ödünç verme
    void kitapOduncVer(string uyeNo, string ISBN) {
        Uye* uye = araUye(uyeler, uyeNo);
        Kitap* kitap = ara(kitaplar, ISBN);

        if (uye && kitap && !kitap->isOduncAlindi()) {
            kitap->setOduncAlindi(true);
            uye->kitapOduncAl(ISBN);
            cout << "Kitap odunc verildi." << endl;
        } else {
            cout << "Islem basarisiz. Uye veya kitap bulunamadi ya da kitap zaten odunc alinmis." << endl;
        }
    }

    // Kitap iade alma
    void kitapIadeAl(string uyeNo, string ISBN) {
        Uye* uye = araUye(uyeler, uyeNo);
        Kitap* kitap = ara(kitaplar, ISBN);

        if (uye && kitap && kitap->isOduncAlindi()) {
            kitap->setOduncAlindi(false);
            uye->kitapIadeEt(ISBN);
            cout << "Kitap iade alindi." << endl;
        } else {
            cout << "Islem basarisiz. Uye veya kitap bulunamadi ya da kitap zaten iade edilmis." << endl;
        }
    }

    // Kitaplarý listeleme
    void kitaplariListele() const {
        cout << "=== KITAP LISTESI ===" << endl;
        for (const auto kitap : kitaplar) {
            kitap->bilgileriGoster();
            cout << "---------------------" << endl;
        }
    }

    // Üyeleri listeleme
    void uyeleriListele() const {
        cout << "=== UYE LISTESI ===" << endl;
        for (const auto uye : uyeler) {
            cout << *uye << endl;
            cout << "Odunc alinan kitaplar: ";
            for (const auto& isbn : uye->getOduncKitaplar()) {
                cout << isbn << " ";
            }
            cout << "\n---------------------" << endl;
        }
    }

    // Dosyadan veri yükleme
    void dosyadanYukle(const string& dosyaAdi) {
        ifstream dosya(dosyaAdi);
        if (!dosya.is_open()) {
            cerr << "Dosya acilamadi!" << endl;
            return;
        }

        string satir;
        while (getline(dosya, satir)) {
            size_t pos = 0;
            string token;
            vector<string> parcalar;

            while ((pos = satir.find(',')) != string::npos) {
                token = satir.substr(0, pos);
                parcalar.push_back(token);
                satir.erase(0, pos + 1);
            }
            parcalar.push_back(satir); // Son parça

            if (parcalar.size() >= 5) {
                if (parcalar.back() == "dergi") {
                    // Dergi kaydý
                    kitapEkle(new Dergi(parcalar[0], parcalar[1], parcalar[2],
                                       stoi(parcalar[3]), stoi(parcalar[5]), parcalar[6]));
                } else {
                    // Normal kitap kaydý
                    Kitap* kitap = new Kitap(parcalar[0], parcalar[1], parcalar[2], stoi(parcalar[3]));
                    kitap->setOduncAlindi(parcalar[4] == "1");
                    kitapEkle(kitap);
                }
            } else if (parcalar.size() >= 3) {
                // Üye kaydý
                Uye* uye = new Uye(parcalar[0], parcalar[1], parcalar[2]);
                for (size_t i = 3; i < parcalar.size(); i++) {
                    uye->kitapOduncAl(parcalar[i]);
                }
                uyeEkle(uye);
            }
        }
        dosya.close();
    }

    // Dosyaya veri kaydetme
    void dosyayaKaydet(const string& dosyaAdi) const {
        ofstream dosya(dosyaAdi);
        if (!dosya.is_open()) {
            cerr << "Dosya acilamadi!" << endl;
            return;
        }

        // Kitaplarý kaydet
        for (const auto kitap : kitaplar) {
            dosya << kitap->dosyaFormatinda() << endl;
        }

        // Üyeleri kaydet
        for (const auto uye : uyeler) {
            dosya << uye->dosyaFormatinda() << endl;
        }

        dosya.close();
        cout << "Veriler basariyla kaydedildi." << endl;
    }
};

// Menü gösterimi
void menuGoster() {
    cout << "\n=== KUTUPHANE OTOMASYONU ===" << endl;
    cout << "1. Yeni Kitap Ekle" << endl;
    cout << "2. Yeni Uye Ekle" << endl;
    cout << "3. Kitap Odunc Ver" << endl;
    cout << "4. Kitap Iade Al" << endl;
    cout << "5. Kitaplari Listele" << endl;
    cout << "6. Uyeleri Listele" << endl;
    cout << "7. Dosyadan Yukle" << endl;
    cout << "8. Dosyaya Kaydet" << endl;
    cout << "0. Cikis" << endl;
    cout << "Seciminiz: ";
}

int main() {
    Kutuphane kutuphane;
    int secim;

    do {
        menuGoster();
        cin >> secim;
        cin.ignore(); // Buffer temizleme

        switch (secim) {
            case 1: {
                string isbn, baslik, yazar;
                int yil;
                char tur;

                cout << "Kitap turu (1: Normal Kitap, 2: Dergi): ";
                cin >> tur;
                cin.ignore();

                cout << "ISBN: "; getline(cin, isbn);
                cout << "Baslik: "; getline(cin, baslik);
                cout << "Yazar: "; getline(cin, yazar);
                cout << "Yayin Yili: "; cin >> yil;
                cin.ignore();

                if (tur == '2') {
                    int sayi;
                    string ay;
                    cout << "Sayi No: "; cin >> sayi;
                    cin.ignore();
                    cout << "Yayin Ayi: "; getline(cin, ay);
                    kutuphane.kitapEkle(new Dergi(isbn, baslik, yazar, yil, sayi, ay));
                } else {
                    kutuphane.kitapEkle(new Kitap(isbn, baslik, yazar, yil));
                }
                break;
            }
            case 2: {
                string no, ad, soyad;
                cout << "Uye No: "; getline(cin, no);
                cout << "Ad: "; getline(cin, ad);
                cout << "Soyad: "; getline(cin, soyad);
                kutuphane.uyeEkle(new Uye(no, ad, soyad));
                break;
            }
            case 3: {
                string uyeNo, isbn;
                cout << "Uye No: "; getline(cin, uyeNo);
                cout << "ISBN: "; getline(cin, isbn);
                kutuphane.kitapOduncVer(uyeNo, isbn);
                break;
            }
            case 4: {
                string uyeNo, isbn;
                cout << "Uye No: "; getline(cin, uyeNo);
                cout << "ISBN: "; getline(cin, isbn);
                kutuphane.kitapIadeAl(uyeNo, isbn);
                break;
            }
            case 5:
                kutuphane.kitaplariListele();
                break;
            case 6:
                kutuphane.uyeleriListele();
                break;
            case 7: {
                string dosyaAdi;
                cout << "Dosya adi: "; getline(cin, dosyaAdi);
                kutuphane.dosyadanYukle(dosyaAdi);
                break;
            }
            case 8: {
                string dosyaAdi;
                cout << "Dosya adi: "; getline(cin, dosyaAdi);
                kutuphane.dosyayaKaydet(dosyaAdi);
                break;
            }
            case 0:
                cout << "Program sonlandiriliyor..." << endl;
                break;
            default:
                cout << "Gecersiz secim!" << endl;
        }
    } while (secim != 0);

    return 0;
}
