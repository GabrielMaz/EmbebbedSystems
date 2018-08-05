/*** BeginHeader */

#define MAX_NUMBER_CONTACTS     5
#define BLOCK_SIZE              23      // array_position 2 bytes, name 11 bytes, phone 10 bytes = 23 bytes

typedef struct Contact {
    int array_position;
    char name[11];
    char phone[10];
};

struct Contact contacts[MAX_NUMBER_CONTACTS];
int contacts_actived;
int position;

/*** EndHeader */

/*** BeginHeader initAgenda */
void initAgenda();
/*** EndHeader */

void initAgenda() {
    int i;
    struct Contact contact, *contact_pointer;
    unsigned int addr;

    contacts_actived = 0;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    printf("\n Agenda:\n\tIniciando agenda de contactos");

    // All contacts by default starts with position -1, its means than there aren't contacts in the agenda
    contact.array_position = -1;

    for (i = 0; i < MAX_NUMBER_CONTACTS; i++) {
        printf(".");
        writeUserBlock(addr, contact_pointer, BLOCK_SIZE);
        addr += BLOCK_SIZE;

        OSTimeDlyHMSM(0,0,0,100);
    }

    printf("\n\nListo! Puede comenzar a usar el programa\n\n");
}

/*** BeginHeader printAgendaEthernet */
void printAgendaEthernet();
/*** EndHeader */

void printAgendaEthernet() {
    struct Contact contact, *contact_pointer;
    char result[80];
    int i;
    unsigned int addr;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    if (contacts_actived == 0) {

        printEthernet("No hay contactos agendados");
        for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
            // Get the contact
            readUserBlock(contact_pointer, addr, BLOCK_SIZE);

            // If not empty, print the info
            if (contact.array_position != -1) {
                sprintf(result, "-------- %d --------\n\nNombre: ", i+1);
                printNameEthernet(contact.name, result);
                strcat(result, "\n\nCelular:");
                printPhoneEthernet(contact.phone, result);
                strcat(result, "\n");
                printEthernet(result);
            }
            // Increase addr to find the next block
            addr += BLOCK_SIZE;
        }     

    } else {
        printEthernet("Lista de contactos:\n\n");

        for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
            // Get the contact
            readUserBlock(contact_pointer, addr, BLOCK_SIZE);

            // If not empty, print the info
            if (contact.array_position != -1) {
                sprintf(result, "-------- %d --------\n\nNombre: ", i+1);
                printNameEthernet(contact.name, result);
                strcat(result, "\n\nCelular:");
                printPhoneEthernet(contact.phone, result);
                strcat(result, "\n");
                printEthernet(result);
            }
            // Increase addr to find the next block
            addr += BLOCK_SIZE;
        }        
    }

    clearScreenEthernet();
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
    char result[10];

    
    for (i = 0; i < 9; i++) {
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
    int result;

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

        clearScreenEthernet();

        // Check if entered value belongs to the possible values
        if (!validateContactPhone(phone_pointer)) {
            printEthernet("Por favor ingrese un celular valido");
            clearScreenEthernet();
            return;
        }

        if(!insertContact(contact_pointer)) {
            printEthernet("No se pudo agregar el contacto");
            clearScreenEthernet();
            return;
        }

        printEthernet("Su contacto fue agregado a la agenda de contactos");
        clearScreenEthernet();
    }
}

/*** BeginHeader insertContact */
int insertContact(struct Contact *newContact);
/*** EndHeader */

int insertContact(struct Contact *newContact) {
    struct Contact contact, *contact_pointer;
    int i, inserted;
    unsigned int addr;

    inserted = 0;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    // Looking for an empty position
    for (i = 0; i < MAX_NUMBER_CONTACTS; i++) {
        // Get the contact
        readUserBlock(contact_pointer, addr, BLOCK_SIZE);

        // If the contact is empty, save the data
        if(contact.array_position == -1) {
            (*newContact).array_position = i;
            contacts_actived ++;
            inserted = 1;

            writeUserBlock(addr, newContact, BLOCK_SIZE);
            return inserted;
        }

        // Increase addr to find the next block
        addr += BLOCK_SIZE;
    }
    return inserted;
}

/*** BeginHeader getContactName */
void getContactName(char *name);
/*** EndHeader */

void getContactName(char *name) {
    printEthernet("\n\nPor favor ingrese un nombre para el contacto (maximo 10 caracteres): ");
    CLEAR_BUFFER();

    // Wait for input
    while(!sock_gets(&socket, name, 11)) { DELAY100MS(); }
    
    CLEAR_SOCKET();

    OSTimeDlyHMSM(0, 0, 0, 100);
}

/*** BeginHeader getContactPhone */
void getContactPhone(char* phone);
/*** EndHeader */

void getContactPhone(char* phone) {

    printEthernet("\n\nPor favor ingrese un celular: ");
    CLEAR_BUFFER();

    // Wait for input
    while(!(sock_gets(&socket, phone, 10))) { DELAY100MS(); }

    CLEAR_SOCKET();

    OSTimeDlyHMSM(0, 0, 0, 100);
}

/*** BeginHeader validateContactPhone */
int validateContactPhone(char *phone);
/*** EndHeader */

int validateContactPhone(char *phone) {
    char aux;
    int i;

    i = 0;

    aux = *phone;

    // For each charactar, c
    for (i = 0; i < 8; i++) {

        // Check if is a digit
        if (!isdigit(*(phone+i))) {
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

    // Convert the input to int
    option = converter(data) - 1;

    CLEAR_SOCKET();

    clearScreenEthernet();

    // Check if option is between 0 and 5 
    if (option < 0 | option > MAX_NUMBER_CONTACTS) {
        printEthernet("Por favor ingrese un dato valido ");
        return;
    }

    deleteContact(option);    
}

/*** BeginHeader deleteContact */
void deleteContact(int option);
/*** EndHeader */

void  deleteContact(int position) {
    struct Contact contact, *contact_pointer;
    int i;
    unsigned int addr;
    char msg[45];

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = BLOCK_SIZE * position;

    if (contacts_actived == 0) {
        printEthernet("No hay contactos para eliminar");
        return;
    }

    // Get the contact
    readUserBlock(contact_pointer, addr, BLOCK_SIZE);
    // Check if it is activated
    if (contact.array_position != -1) {
        // Set it deleted
        contact.array_position = -1;
        // Decrease the number of contacts in agenda
        contacts_actived --;
        // Update the block
        writeUserBlock(addr, contact_pointer, BLOCK_SIZE);
        sprintf(msg, "\nSe ha eliminado correctamente a\n");
        strcat(msg, contact.name);
        printEthernet(msg);
    } else {
        printEthernet("\nIngrese una opcion valida\n");
    }
}

/*** BeginHeader friendlyPhone */
int friendlyPhone(char *phone);
/*** EndHeader */

int friendlyPhone(char *phone) {
    struct Contact contact, *contact_pointer;
    char result[80];
    int i, isFriend;
    unsigned int addr;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    isFriend = 0;

    for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
        // Get the contact
        readUserBlock(contact_pointer, addr, BLOCK_SIZE);

        // If not empty, print the info
        if (contact.array_position != -1) {
            // Check if the number is in agenda
            if (equalsPhones(contact.phone, phone)) {
                isFriend = 1;
            }
        }
        // Increase addr to find the next block
        addr += BLOCK_SIZE;
    }
    
    return isFriend;
}

/*** BeginHeader equalsPhones */
int equalsPhones(char *phone1, char *phone2);
/*** EndHeader */

int equalsPhones(char *phone1, char *phone2) {
    int i, result;

    result = 1;

    for (i = 0; i < 9; i++) {
        if ((*phone1+i) != (*phone2+i)) {
            result = 0;
            break;
        }
    }

    return result;
}