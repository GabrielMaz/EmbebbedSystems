/*** BeginHeader */

#define MAX_NUMBER_CONTACTS 7

typedef struct Contact {
    int array_postion;
    char name[11];
    char phone[9];
};

struct Contact contacts[MAX_NUMBER_CONTACTS];
int contacts_actived;

/*** EndHeader */

/*** BeginHeader initAgenda */
void initAgenda();
/*** EndHeader */

void initAgenda() {
    int i;
    struct Contact contact;

    for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
        contact.array_postion = -1;
        contacts[i] = contact;
    }
}


/*** BeginHeader printAgendaEthernet */
void printAgendaEthernet();
/*** EndHeader */

void printAgendaEthernet() {
    struct Contact contact;
    int i;
    char result[80];

    if (contacts_actived == 0) {
        printEthernet("No hay contactos agendados\n\n");

    } else {
        printEthernet("Listado de contactos:\n\n");

        for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
            contact = contacts[i];

            if (contact.array_postion != -1) {
                sprintf(result, "-------- %d --------\n\nNombre: ", i+1);
                printNameEthernet(contact.name, result);
                strcat(result, "\n\nCelular:");
                printPhoneEthernet(contact.phone, result);
                strcat(result, "\n");
                printEthernet(result);
            }
        }        
    }
}

/*** BeginHeader printNameEthernet */
void printNameEthernet(char data[], char resultEthernet[]);
/*** EndHeader */

void printNameEthernet(char data[], char resultEthernet[]) {
    int i;
    char result[11];

    i = 0;

    while((data[i]) != '\0'){ 
        result[i] = data[i];
        i++;
        if (i == 10) {
            break;
        }
    }

    strcat(resultEthernet, result);
}

/*** BeginHeader printPhoneEthernet */
void printPhoneEthernet(char data[], char resultEthernet[]);
/*** EndHeader */

void printPhoneEthernet(char data[], char resultEthernet[]) {
    int i;
    char result[9];

    
    for (i = 0; i < 8; i++) {
        result[i] = data[i];
    }

    strcat(resultEthernet, result);
}

/*** BeginHeader addContact */
void addContact();
/*** EndHeader */

void addContact() {
    struct Contact contact, *contact_pointer;
    char *name_pointer, *phone_pointer;

    contact_pointer = &contact;
    name_pointer = (*contact_pointer).name;
    phone_pointer = (*contact_pointer).phone;

    // Check if the agenda is full of contacts
    if (contacts_actived == MAX_NUMBER_CONTACTS) {
        clearScreenEthernet();
        printEthernet("No hay espacio para mas contactos\n\n");

    } else {
        // Ask for a contact name
        getContactName(name_pointer);

        // Ask for a phone
        getContactPhone(phone_pointer);

        // Check if entered value belongs to the possible values
        if (!validateContactPhone(phone_pointer)) {
            clearScreenEthernet();
            printEthernet("Por favor ingrese un dato valido");
            
            return;
        }

        insertContact(contact_pointer);

        clearScreenEthernet();
        printEthernet("Su contacto fue agregado a la agenda de contactos");
        clearScreenEthernet();
    }
}

/*** BeginHeader insertContact */
void insertContact(struct Contact *contact);
/*** EndHeader */

void insertContact(struct Contact *contact) {
    int i;
    unsigned long addr, blockSize;

    // Initial position to userBlock
    addr = 0;

    // Size of each block
    // array_position 2 bytes, name 11 bytes, phone 9 bytes
    blockSize = 22;

    // Looking for an empty position
    for (i = 0; i < MAX_NUMBER_CONTACTS; i++) {
        if (contacts[i].array_postion == -1) {
            (*contact).array_postion = i;
            contacts[i] = (*contact);
            contacts_actived += 1;
            break;
        }

        writeUserBlock(addr, &contacts[i].array_postion, 2);
        writeUserBlock(addr+2, contacts[i].name, 11);
        writeUserBlock(addr+13, contacts[i].array_postion, 9);

        addr += blockSize;
    }
}

/*** BeginHeader getContactName */
void getContactName(char *name);
/*** EndHeader */

void getContactName(char *name) {
    printEthernet("\nPor favor ingrese un nombre para el contacto (maximo 10 caracteres): ");
    CLEAR_BUFFER();

    // Wait for input
    while(!sock_gets(&socket, name, 11)) { DELAY100MS(); }
    
    CLEAR_SOCKET();
}

/*** BeginHeader getContactPhone */
void getContactPhone(char* phone);
/*** EndHeader */

void getContactPhone(char* phone) {

    printEthernet("\nPor favor ingrese un celular: ");
    CLEAR_BUFFER();

    // Wait for input
    while(!(sock_gets(&socket, phone, 9))) { DELAY100MS(); }

    CLEAR_SOCKET();
}

/*** BeginHeader validateContactPhone */
int validateContactPhone(char *phone);
/*** EndHeader */

int validateContactPhone(char *phone) {
    char aux;

    i = 0;

    aux = *phone;

    // For each charactar, c
    for (i = 0; i < 8; i++) {

        // Check if is a digit
        if (!isDigit(*(phone+i))) {
            return 0;
        }
    }

    return 1;
}

/*** BeginHeader deleteContactEthernet */
void deleteContactEthernet();
/*** EndHeader */

void deleteContactEthernet() {
    char data[4];
    int option, i;

    printEthernet("\nIngrese numero de contacto a eliminar: ");
    CLEAR_BUFFER();

    // Wait for input
    while(!sock_gets(&socket, data, 4)) { DELAY100MS(); }
    option = converter(data) - 1;

    CLEAR_SOCKET();

    // Check if option is not between 0 and 5 
    if (option < 0 | option > MAX_NUMBER_CONTACTS) {
        clearScreenEthernet();
        printEthernet("Por favor ingrese un dato valido ");
        return;
    }

    // Check if the contact could be deleted
    if (deleteContact(option)) {
        clearScreenEthernet();
        printEthernet("El contacto fue eliminado correctamente");

    } else {
        clearScreenEthernet();
        printEthernet("No existe un contacto con ese numero");
    }    
}

/*** BeginHeader deleteContact */
int deleteContact(int option);
/*** EndHeader */

int deleteContact(int option) {
    int i, deleted, addr, blockSize;

    // Initial position to userBlock
    addr = 0;

    // Size of each block
    // array_position 2 bytes, name 11 bytes, phone 9 bytes
    blockSize = 22;

    deleted = 0;

    // Search the contact to delete
    for (i = 0; i < MAX_NUMBER_CONTACTS; i++) {

        // When found it, set the array_position to -1, so it is deleted or "invisible"
        if (i == option & contacts[i].array_postion != -1) {
            contacts[i].array_postion = -1;
            contacts_actived -= 1;
            deleted = 1;

            writeUserBlock(addr, &contacts[i], 22);
            break;
        }

        addr += blockSize;
    }

    return deleted;
}