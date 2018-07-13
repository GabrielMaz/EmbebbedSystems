#define MAX_NUMBER_CONTACTS     2
#define BLOCK_SIZE              23      // array_position 2 bytes, name 11 bytes, phone 10 bytes = 23 bytes

typedef struct Contact {
    int array_position;
    char name[11];
    char phone[10];
};

struct Contact contacts[MAX_NUMBER_CONTACTS];
int contacts_actived;
int position;

void delay(int ms_delay){
    auto unsigned long t0;

    // ms timer is updated by rabbit
    t0 = MS_TIMER;

    while (MS_TIMER < (t0 + ms_delay));
}

int multipleOfTen(int multiple) {
    int i, result;

    result = 1;

    for (i = 0; i < multiple; i++) {
        result *= 10;
    }

    return result;
}

int converter(char *data) {
    int i, length, result;

    i = 0;
    length = 0;
    result = 0;

    // get the length of the string
    while(*(data+i) != '\0'){
        length++;
        i++;
    }

    i = 0;

    while (*(data+i) != '\0') {

        // get value of string
        *(data + i) = *(data + i) - 0x30;                       //0x30 hex = 48 ascii = 0

        // increase depending on the position of the number | 123 = 100*3 + 10*2 + 1*3
        result += multipleOfTen(length - i - 1)*(*(data+i));

        i++;
    }

    return result;
}

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

void initAgenda() {
    int i;
    struct Contact contact, *contact_pointer;
    unsigned int addr;

    contacts_actived = 0;
    
    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;

    // All contacts by default starts with position -1
    contact.array_position = -1;

    for (i = 0; i < MAX_NUMBER_CONTACTS; i++) {
        writeUserBlock(addr, contact_pointer, BLOCK_SIZE);

        addr += BLOCK_SIZE;
    }
}

void printPhone(char *phone) {
    int i;

    for (i = 0; i < 9; i++) {
        printf("%c",*(phone+i));
    }
}

void printAgenda() {
    struct Contact contact, *contact_pointer;
    int i;
    unsigned int addr;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;    

    if (contacts_actived == 0) {
        printf("No hay contactos agendados\n\n");

    } else {
        printf("Listado de contactos:\n\n");

        for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
            readUserBlock(contact_pointer, addr, BLOCK_SIZE);
            if (contact.array_position != -1) {
                printf("\n\n Posicion: %d\n Nombre: %s\n Celular: %s", 
                (*contact_pointer).array_position, (*contact_pointer).name, (*contact_pointer).phone);
                printf("\n\n");
            }
            addr += BLOCK_SIZE;
        }      
    }
}

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

void addContact() {
    struct Contact contact, *contact_pointer;
    char *name_pointer, *phone_pointer;
    int result;

    contact_pointer = &contact;

    // Check if the agenda is full of contacts
    if (contacts_actived == MAX_NUMBER_CONTACTS) {
        printf("No hay espacio para mas contactos\n\n");

    } else {
        // Ask for a contact name
        //getContactName(name_pointer);
        printf("\nPor favor ingrese un nombre para el contacto (maximo 10 caracteres): ");
        while(!getswf(contact.name));

        // Ask for a phone
        //getContactPhone(phone_pointer);
        printf("\nPor favor ingrese un celular: ");
        while(!getswf(contact.phone));

        // Check if entered value belongs to the possible values
        if (!validateContactPhone(contact.phone)) {

            printf("\nPor favor ingrese un celular valido\n\n");
            
            return;
        }

        if(!insertContact(contact_pointer)) {
            printf("\nNo se pudo agregar el contacto\n\n");
            
            return;
        }

        printf("\nSu contacto fue agregado a la agenda de contactos\n\n");
    }
}

void deleteContact(int position) {
    struct Contact contact, *contact_pointer;
    int i;
    unsigned int addr;

    contact_pointer = &contact;

    // Initial position to userBlock
    addr = 0;    

    if (contacts_actived == 0) {
        printf("No hay contactos para eliminar\n\n");

    } else {
        printf("Listado de contactos:\n\n");

        for (i = 0; i < MAX_NUMBER_CONTACTS; i++){
            readUserBlock(contact_pointer, addr, BLOCK_SIZE);
            if (contact.array_position != -1) {
                printf("\n\n Posicion: %d\n Nombre: %s\n Celular: %s", 
                (*contact_pointer).array_position, (*contact_pointer).name, (*contact_pointer).phone);
                printf("\n\n");
            }
            addr += BLOCK_SIZE;
        }      
    }
}

void main (void) {
    char data[2];
    int option;

    initAgenda();

    while(1) {
        printf("\nMenu: \n\n1 - Ver contactos \n\n2 - Agregar contacto \n\n3 - Eliminar contacto \n\n4 - Ubicacion del vehiculo\n\nSeleccione una opcion:");
        memset(data, 0, 2);
        while(!getswf(data));

        printf("\n");
        
        option = converter(data);

        switch(option) {
            case 1:
                printAgenda();
                break;
            
            case 2:
                addContact();
                break;
            
            case 3:

                break;

            
            case 4:

                break;
            
        }
    }
}