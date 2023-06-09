#include <EEPROM.h>
#include <SpritzCipher.h>

const byte Key[8] = {'a','h','p','r','s','t','d','8'};

struct Contact {
  char name[30];
  char number[15];
};

const int EEPROM_SIZE = 1024;  // Total size of EEPROM
const int CONTACT_SIZE = sizeof(Contact);  // Size of each contact entry
const int MAX_CONTACTS = EEPROM_SIZE / CONTACT_SIZE;  // Maximum number of contacts

void setup() {
  Serial.begin(9600);

  // Display the menu
  displayMenu();
}

void loop() {
  // Main program loop
  if (Serial.available()) {
    int choice = Serial.parseInt();
    processChoice(choice);
  }
}

void displayMenu() {
  Serial.println("\nMenu:");
  Serial.println("1. Create Contact");
  Serial.println("2. Read Contacts");
  Serial.println("3. Update Contact");
  Serial.println("4. Delete Contact");
  Serial.println("Enter your choice (1-4):");
}

void processChoice(int choice) {
  switch (choice) {
    case 1:
      createContact();
      break;
    case 2:
      readContacts();
      break;
    case 3:
      updateContact();
      break;
    case 4:
      deleteContact();
      break;
    default:
      Serial.println("Invalid choice!");
      break;
  }

  displayMenu();
}

void createContact() {
  Serial.println("Enter name:");
  while (Serial.available() == 0) {}
  String name = Serial.readString();
  Serial.println("Enter number:");
  while (Serial.available() == 0) {}
  String number = Serial.readString();

  Contact newContact;
  strncpy(newContact.name, name.c_str(), sizeof(newContact.name) - 1);
  strncpy(newContact.number, number.c_str(), sizeof(newContact.number) - 1);

  int index = findEmptyContactIndex();
  if (index == -1) {
    Serial.println("Error: EEPROM is full, cannot create contact!");
    return;
  }

  int address = index * CONTACT_SIZE;
  EEPROM.put(address, newContact);

  Serial.println("Contact created successfully!");
}

void readContacts() {
  Serial.println("Reading contacts:");

  for (int index = 0; index < MAX_CONTACTS; index++) {
    Contact contact;
    int address = index * CONTACT_SIZE;
    EEPROM.get(address, contact);

    if (isEmptyContact(contact)) {
      continue;
    }

    Serial.print("Index: ");
    Serial.println(index);
    Serial.print("Name: ");
    Serial.println(contact.name);
    byte byteArray[8];
    memset(byteArray, 0, sizeof(byteArray));
    strcpy((char *)byteArray,contact.name);
    crypt(byteArray, sizeof(byteArray), Key, sizeof(Key));
    
    Serial.print("Number: ");
    Serial.println(contact.number);
    memset(byteArray, 0, sizeof(byteArray));
    strcpy((char *)byteArray,contact.number);
    crypt(byteArray, sizeof(byteArray), Key, sizeof(Key));
  }
}
void crypt(const byte *msg, byte msgLen, const byte *key, byte keyLen){
  spritz_ctx s_ctx;
  byte buf[8]; /* Output buffer */
  unsigned int i;

  spritz_setup(&s_ctx, key, keyLen);
  spritz_crypt(&s_ctx, msg, msgLen, buf);

  /* Print Ciphertext */
  Serial.print("encrypt: ");
  for (i = 0; i < msgLen; i++) {
    if (buf[i] < 0x10) { /* To print "0F" not "F" */
      Serial.write('0');
    }
    Serial.print(buf[i], HEX);
  }
  Serial.println();

  spritz_setup(&s_ctx, key, keyLen);
  spritz_crypt(&s_ctx, buf, msgLen, buf);

  /* Print MSG after decryption */
  Serial.print("decrypt: ");
  for (i = 0; i < msgLen; i++) {
    Serial.write(buf[i]);
  }
  Serial.println();

  /* Check the output */
  if (spritz_compare(buf, msg, msgLen)) {
    Serial.println("Output != Test_Vector");
  }
  Serial.println();
}

void updateContact() {
  Serial.println("Enter contact index to update:");
  while (Serial.available() == 0) {}
  String input = Serial.readString();
  int index = input.toInt();

  if (!isValidIndex(index)) {
    Serial.println("Error: Invalid contact index!");
    return;
  }

  Serial.println("Enter new name:");
  while (Serial.available() == 0) {}
  String name = Serial.readString();
  Serial.println("Enter new number:");
  while (Serial.available() == 0) {}
  String number = Serial.readString();

  int address = index * CONTACT_SIZE;
  Contact updatedContact;
  strncpy(updatedContact.name, name.c_str(), sizeof(updatedContact.name) - 1);
  strncpy(updatedContact.number, number.c_str(), sizeof(updatedContact.number) - 1);

  EEPROM.put(address, updatedContact);

  Serial.println("Contact updated successfully!");
}

void deleteContact() {
  Serial.println("Enter contact index to delete:");
  while (Serial.available() == 0) {}
  String input = Serial.readString();
  int index = input.toInt();

  if (!isValidIndex(index)) {
    Serial.println("Error: Invalid contact index!");
    return;
  }

  int address = index * CONTACT_SIZE;
  Contact emptyContact;
  memset(&emptyContact, 0, sizeof(emptyContact));

  EEPROM.put(address, emptyContact);

  Serial.println("Contact deleted successfully!");
}

int findEmptyContactIndex() {
  for (int index = 0; index < MAX_CONTACTS; index++) {
    int address = index * CONTACT_SIZE;
    Contact contact;
    EEPROM.get(address, contact);

    if (isEmptyContact(contact)) {
      return index;
    }
  }

  return -1;  // No empty slot found
}

bool isEmptyContact(const Contact& contact) {
  return contact.name[0] == '\0' && contact.number[0] == '\0';
}

bool isValidIndex(int index) {
  return index >= 0 && index < MAX_CONTACTS;
}
