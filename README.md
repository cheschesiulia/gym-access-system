Gym System IoT - README
Descriere
Proiect PR IoT - Gym System este un sistem IoT care monitorizează și controlează accesul într-o sală de sport. Sistemul folosește un cititor RFID pentru validarea membrilor, colectează date de intrare/ieșire și permite controlul unui ventilator pe baza numărului de persoane din sală, gestionat printr-o aplicație web accesibilă personalului.

Scopul Proiectului
Acest proiect își propune să creeze un sistem IoT eficient pentru controlul accesului și gestionarea condițiilor de mediu dintr-o sală de sport, contribuind la confortul membrilor și la optimizarea resurselor disponibile.

Componente Hardware
ESP32: Microcontroller care gestionează conectivitatea Wi-Fi și dispozitivele IoT (senzor RFID, ventilator).
Senzor RFID (PN532): Senzor pentru identificarea membrilor prin scanarea cardurilor RFID.
Ventilator: Acționează în funcție de numărul de persoane din sală.
LED-uri: Indică accesul permis (verde) sau refuzat (roșu).
Tranzistor 2N2222: Controlul ventilatorului.
Fire Dupont: Conectivitate între componente.

Componente Software
Flask Server: Un server Python care gestionează logica de aplicație și comunicarea cu ESP32.
Aplicație Web (Dashboard): Interfață grafică care permite personalului să vizualizeze datele și să controleze ventilatorul.

Flow - Accesul membrilor în sală

Membrii își scanează cardurile RFID pe senzorul conectat la ESP32.
ESP32 trimite un HTTP GET request către serverul Flask pentru a valida accesul.
Serverul Flask validează membrul și permite accesul (LED verde) sau refuză (LED roșu).

  
Flow - Monitorizarea numărului de membri și controlul ventilatorului
  
Serverul Flask actualizează log-urile în timp real cu informațiile de intrare/ieșire.
Dashboard-ul web permite personalului să vizualizeze numărul de persoane în sală și să controleze ventilatorul în funcție de acest număr.

Puncte Forte

Monitorizare în timp real: Vizualizare instantanee a numărului de membri din sală.
Automatizare: Controlul ventilatorului se face automat pe baza numărului de persoane.
Statistici: Grafice care indică orele de vârf ale salii.

Posibilități de Extindere
Notificări pentru utilizatori: Notificări despre orele de vârf ale salii.
Limitarea accesului: Setarea unei limite pentru a preveni supra-aglomerarea.

Provocări și Soluții

Problema: Modulul de economisire a energiei al Wi-Fi interferează cu comunicarea SPI.
Soluție: Dezactivarea modului de economisire și setarea unei viteze mai mici pentru SPI.
Securizarea conexiunii HTTPS:

Problema: Utilizarea fingerprint-ului certificatului nu a funcționat.
Soluție: Utilizarea directă a fișierului crt al certificatului SSL.
Probleme de sincronizare între hardware și software:

Problema: Datele despre numărul de persoane nu erau actualizate rapid pe dashboard.
Soluție: Optimizarea intervalului de actualizare a datelor.
