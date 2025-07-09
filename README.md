# 🏭 Warehouse Location Problem Solver (WLP)

Bu proje, **Warehouse Location Problem (WLP)** için geliştirilmiş, C++ diliyle yazılmış bir çözüm algoritması içerir. Amaç; müşteri taleplerini depo kapasiteleri ve kurulum maliyetlerini dikkate alarak en düşük toplam maliyetle karşılamaktır.

## 📌 Problem Tanımı

Verilenler:
- `W` adet depo, her birinin kapasitesi ve kurulum maliyeti vardır.
- `C` adet müşteri ve her birinin talebi mevcuttur.
- Her müşteri için, her depodan tedarik etmenin birim maliyeti verilmiştir.

Amaç:
> Müşteri taleplerini, depo kapasitesi ve kurulum maliyetlerini dikkate alarak, toplam tedarik + kurulum + denge cezası maliyetlerini **minimuma indirmek**.

---

## ⚙️ Kullanılan Yöntem

- **Greedy Başlangıç Çözümü**
- **Local Search (Yerel Arama)**
- **Iterated Local Search (ILS)**
- **Denge cezası (imbalance penalty)** ile yük dağılımı dengelenmiştir.

---
📊 Depo Kullanım Raporu (Terminal Çıktısı Örneği)
📊 Depo Kullanım Dağılımı:
Depo	Kapasite	Kullanım	Müşteri	Dolu %
0	    500	        430	        12	    86.0%
3	    600	        590	        15	    98.3%
...

🧪 Test Verileri
Veri setleri data/ klasöründe yer alır. Örnek:

wl_100.txt

wl_200.txt

wl_500.txt

