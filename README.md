## Json-parser

Lo scopo del progetto è scrivere un parser (tramite grammatiche context-free) di file in formato json. Il contenuto del file deve essere salvato in un container C++ che permetta l’accesso (tramite iteratori e overloading di operatori) del contenuto letto dal file json.

## 1. Formato json

Si vuole scrivere un parser per una variante _semplificata_ di json – un formato testuale per lo scambio di informazione sul Web.

Un file json contiene dati in una struttura ad albero (ricorsiva), simile al formato XML. A differenza di XML, il formato json ammette sia **liste** di valori che **dizionari** (definiti nel dettaglio qui sotto). Globalmente, un file json è una lista, un dizionario, un numero (double), un booleano, una stringa, oppure null.

### Liste

Una lista è una sequenza di valori eterogenei separati da virgola e racchiusa tra due parentesi quadre. I valori contenuti nella lista possono essere di **sei** tipi: una lista (ricorsione), un dizionario (definito sotto), una stringa racchiusa tra doppi apici, un valore di tipo double, bool (true/false), oppure null (che sta ad indicare l’assenza di un valore).

Il seguente è un esempio di un semplice file json di tipo lista (ossia, l’intero file è una lista), contenente due stringhe, un valore di tipo double (34.5), seguito da un valore di tipo bool (true), seguito da null, seguito da una lista.

**Nota.** I separatori (newline, tab, spazi) vanno ignorati. L’unica eccezione sono i separatori racchiusi tra doppi apici  (per esempio, lo spazio in "prima stringa"): in questo caso, i separatori devono fare parte della stringa estratta dal file. 

### Una nota importante sulle stringhe

Tutti i caratteri tra le doppie virgolette vanno estratti tali e quali. Tenete presente che le stringhe possono contenere il carattere `"` preceduto da un carattere di `\`, i.e., la stringa potrebbe contenere la sotto stringa `\"`. In questo caso, quindi, la stringa **non** termina con quella occorrenza di `"`. Le stringhe terminano solo quando il carattere `"` **non** è preceduto da `\`.

**Nota**: I caratteri speciali `\n` (e simili, tipo `\t`, `\r`, etc.) non fanno eccezione! Se il file json contiene una stringa in cui compare `\n`, ad esempio, vanno estratti **due** caratteri: backslash (`\`) seguito dal carattere `n`, **non** va estratto il singolo carattere `\n` (che significa newline). Questa è una modifica rispetto al formato originale che abbiamo introdotto per semplificare la scrittura del parser.

### Dizionari

Un dizionario è un insieme (una lista) di coppie

**chiave : valore**

separate da virgola e racchiuse da due parentesi graffe. Notare che chiave e valore sono separate da un carattere ‘:’ (due punti). Le chiavi possono essere solo stringhe racchiuse tra doppi apici (di nuovo, queste stringhe possono contenere caratteri escape ‘\\’). I valori possono essere, come sopra, una lista, un dizionario, una stringa racchiusa tra doppi apici (possibilmente con caratteri escape ‘\\’), un valore double o bool, oppure null.

## 2. Container

Lo scopo del progetto è scrivere un container

    class json;

che permetta di immagazzinare tutte le informazioni contenute in un file json. Il parser json realizzato verrà usato per estrarre dati da un file json e costruire un oggetto di tipo `json`.

### Metodi pubblici di json

#### Metodi bool

I seguenti metodi possono essere utilizzati per capire di che tipo è l’oggetto `json` (lista, dizionario, stringa, numero, booleano, oppure null). Nota bene: esattamente **uno** di questi metodi deve restituire true (l’oggetto `json` è esattamente uno di questi sei tipi).

---
    bool json::is_list() const;
Questo metodo restituisce true se e solo se l’oggetto è una lista.

---
    bool json::is_dictionary() const;
Questo metodo restituisce true se e solo se l’oggetto è un dizionario.

---  
    bool json::is_string() const;
Questo metodo restituisce true se e solo se l’oggetto è una stringa.

---
    bool json::is_number() const;

Questo metodo restituisce true se e solo se l’oggetto è un numero (double).

---
    bool json::is_bool() const;
Questo metodo restituisce true se e solo se l’oggetto è un booleano.

---
    bool json::is_null() const;
Questo metodo restituisce true se e solo se l’oggetto è null.


#### Metodi per accedere (dall’esterno) al contenuto del container

    json const& json::operator[](std::string const& key) const;
    json& json::operator[](std::string const& key);

Se l’oggetto è un dizionario (`is_dictionary()` restituisce `true`), questo metodo restituisce una reference all’elemento `json` associato alla chiave `key`.


Se il dizionario non contiene nessuna coppia chiave-valore la cui chiave è uguale a `key`, allora:

- il metodo `operator[]` deve inserire nel dizionario una nuova coppia chiave-valore la cui chiave è `key` e il cui valore è un `json` costruito con il costruttore di default (i.e., un `json` il cui contenuto è `null`), ed infine restituire una reference a questo nuovo `json` appena costruito;
- il metodo `operator[] const` deve lanciare invece un'eccezione non potendo effettuare nessun inserimento.

In ogni caso, se l’oggetto non è un dizionario, lanciamo un’eccezione `json_exception` (con messaggio `msg` a piacere)
se il metodo `operator[]` viene invocato.

---
 
Se l’oggetto è una lista (`is_list()` restituisce `true`), usiamo gli iteratori `list_iterator` e `const_list_iterator` per accedere ai membri della lista (a loro volta, degli oggetti `json`).

I seguenti metodi permettono di ottenere iteratori all’inizio e alla fine della lista. Se `is_list()` restituisce `false`, questi metodi devono lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere). 

**Attenzione.** L’ordine degli elementi nella lista deve essere lo stesso del file letto in input. In particolare, gli iteratori devono restituire gli elementi in questo ordine.

    json::list_iterator json::begin_list();
    json::const_list_iterator json::begin_list() const;
    json::list_iterator json::end_list();
    json::const_list_iterator json::end_list() const;

---

In modo analogo, se l’oggetto è un dizionario (`is_dictionary()` restituisce `true`), usiamo gli iteratori `dictionary_iterator` e `const_dictionary_iterator` per accedere alle coppie contenute nel dizionario (le coppie sono di tipo `std::pair<std::string,json>`). I seguenti metodi permettono di ottenere iteratori all’inizio e alla fine del dizionario. Se  `is_dictionary()` restituisce `false`, questi metodi devono lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere).

**Attenzione.** A differenza degli iteratori su lista, in questo caso non è importante l’ordine in cui vengono immagazzinati e visitati gli elementi del dizionario.

    json::dictionary_iterator json::begin_dictionary();
    json::const_dictionary_iterator json::begin_dictionary() const;
    json::dictionary_iterator json::end_dictionary();
    json::const_dictionary_iterator json::end_dictionary() const;

  
---

Se `is_number()` è `true` (l’oggetto è un numero), i metodi qui sotto restituiscono una reference / const reference alla variabile `double` contenuta nel `json` (e che potete salvare in `json::impl`). Se `is_number()` è `false`, questo metodo deve lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    double& json::get_number();
    double const& json::get_number() const;

  
---

Se `is_bool()` è `true` (l’oggetto è un boolean), questi metodi restituiscono una reference / const reference alla variabile booleana contenuta nel `json` (e che potete salvare in `json::impl`). Altrimenti, viene lanciata un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    bool& json::get_bool();
    bool const& json::get_bool() const;

---

Se `is_string()` è `true` (l’oggetto è una stringa), questi metodi restituiscono una reference / const reference alla variabile di tipo stringa stringa contenuta nel `json` (e che potete salvare in `json::impl`). Altrimenti, viene lanciata un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    std::string& json::get_string();
    std::string const& json::get_string() const;

 
 
#### Metodi per settare il contenuto del json

Questo metodo rende il json di tipo stringa, rendendo la sua stringa memorizzata internamente uguale a `x` e cancellando gli eventuali dati precedentemente memorizzati nel json (per esempio, se il json era di tipo lista, questa funzione deve svuotare la lista e poi memorizzare la stringa `x`). Dopo la chiamata di questa funzione, `is_string()` deve restituire `true` (nessun altro metodo booleano deve restituire `true`).

    void json::set_string(std::string const& x);

--- 

Questo metodo rende il json di tipo bool, rendendo il bool memorizzato internamente uguale a `x` e cancellando gli eventuali dati precedentemente memorizzati nel json (per esempio, se il json era di tipo lista, questa funzione deve svuotare la lista e poi memorizzare il booleano `x`). Dopo la chiamata di questa funzione, `is_bool()` deve restituire `true` (nessun altro metodo booleano deve restituire `true`).

    void json::set_bool(bool x);

---

Questo metodo rende il json di tipo numero, rendendo il double memorizzato internamente uguale a `x` e cancellando gli eventuali dati precedentemente memorizzati nel json (per esempio, se il json era di tipo lista, questa funzione deve svuotare la lista e poi memorizzare il double `x`). Dopo la chiamata di questa funzione, `is_number()` deve restituire true (nessun altro metodo booleano deve restituire `true`).

    void json::set_number(double x);

---

Questo metodo rende il json di tipo null, cancellando gli eventuali dati precedentemente memorizzati nel json (per esempio, se il json era di tipo lista, questa funzione deve svuotare la lista e poi memorizzare il fatto che l’oggetto è un json null). Dopo la chiamata di questa funzione, `is_null()` deve restituire `true` (nessun altro metodo booleano deve restituire `true`).

    void json::set_null();

---

Questo metodo rende il json di tipo lista, rendendo la lista interna uguale alla lista vuota  e cancellando gli eventuali dati precedentemente memorizzati nel json, _anche nel caso il json fosse una lista_ (per esempio, se il json era di tipo lista, questa funzione deve rimpiazzare la lista con la lista vuota). Dopo la chiamata di questa funzione, `is_list()` deve restituire `true` (nessun altro metodo booleano deve restituire `true`).

    void json::set_list();

---


Questo metodo rende il json di tipo dizionario, rendendo il dizionario interno uguale al dizionario vuoto  e cancellando gli eventuali dati precedentemente memorizzati nel json, _anche nel caso il json fosse un dizionario_ (per esempio, se il json era di tipo lista, questa funzione deve svuotare la lista e inizializzare il dizionario a quello vuoto). Dopo la chiamata di questa funzione, `is_dictionary()` deve restituire `true` (nessun altro metodo booleano deve restituire `true`).

    void json::set_dictionary();

---

Se il `json` è di tipo lista (`is_list()` restituisce true), questo metodo aggiunge `x` in testa alla lista. 

Se il `json` non è di tipo lista, questa funzione deve lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    void json::push_front(json const& x);

---

Se il `json` è di tipo lista (`is_list()` restituisce `true`), questo metodo aggiunge `x` in fondo alla lista. 

Se il `json` non è di tipo lista, questa funzione deve lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    void json::push_back(json const& x);

---

Se il `json` è di tipo dizionario (`is_dictionary()` restituisce `true`), questo metodo aggiunge la coppia chiave-valore `x` nel dizionario. Il metodo non deve verificare se esiste già una coppia nel dizionario la cui chiave è `x.first`.

Se il `json` non è di tipo dizionario, questa funzione deve lanciare un’eccezione `json_exception` (con messaggio `msg` a piacere). 

    void json::insert(std::pair<std::string,json> const& x);

  

### Metodi esterni (non membri di json)


    std::ostream& operator<<(std::ostream& lhs, json const& rhs);

Questo metodo scrive l’oggetto `rhs` sull’output stream `lhs` in formato json valido (vedi descrizione formato json all’inizio del documento). Nota: i separatori (spazi, tab, newlines) sono opzionali nel formato json (potete scegliere se metterli o no: è equivalente). 

---

    std::istream& operator>>(std::istream& lhs, json& rhs);

Questo metodo legge da `lhs` un oggetto `json` e lo salva in `rhs` (sovrascrivendone il contenuto). Questa è la funzione che lancia il parser json realizzato: l’input stream è uno stream di caratteri che contiene un documento in formato json (come descritto all’inizio). L’operatore `>>` deve estrarre i dati dallo stream (usando il parser) e costruire l’oggetto `json` che contiene quei dati (cancellando il contenuto precedente di `rhs`, se questi sono presenti).

Se la funzione riscontra dei problemi durante il parsing, deve venire lanciata un’eccezione `json_exception` (con messaggio `msg` a piacere). Cercate di rilevare il numero più elevato possibile di errori di parsing (testeremo il vostro codice anche su file json in formato errato, verificando che venga lanciata l’eccezione).
  


## 3. Esempio

Il container json creato può essere usato come un editor di file in formato json. Una volta caricato, il file può essere manipolato/navigato usando gli operatori definiti. Per esempio, supponiamo che `std::cin` contenga lo stream:

    [
        1,
        {
            "prima chiave" : 5,
            "seconda chiave" : [4.12,2,true],
            "terza chiave" : "una stringa",
            "quarta chiave" : {"a" : 4, "b" : [4,5]}
        },
        3
    ]

allora, il seguente codice è valido e deve stampare a schermo “4”: 

    json j;
    std::cin >> j;
    json& y = *(++j.begin_list());
    std::cout << y["quarta chiave"]["a"];


Inoltre, la classe ci permette di modificare un oggetto `json`. Per esempio, se la variabile 

    json z;

contiene i dati del file json

    {"c" : 5, "d" : 6}

e `j` è la variabile di tipo `json` definita sopra, allora dopo l’assegnamento

    (*(++j.begin_list()))["prima chiave"] = z

 
La variabile `j` deve contenere i seguenti dati: 

    [
        1,
        {
            "prima chiave" : {"c" : 5, "d" : 6},
            "seconda chiave" : [4.12,2,true],
            "terza chiave" : "una stringa",
            "quarta chiave" : {"a" : 4, "b" : [4,5]}
        },
        3
    ]
