#include <EEPROM.h>
#include <MD5.h>

struct Contact {
  char Name[30];
  char number[15];
};

const int EEPROM_SIZE = 1024;  // Total size of EEPROM
const int CONTACT_SIZE = sizeof(Contact);  // Size of each contact entry
const int MAX_CONTACTS = EEPROM_SIZE / CONTACT_SIZE;  // Maximum number of contacts

void setup() {
  Serial.begin(9600);

  // Uncomment the following line to create an initial contact
  createContact("John Doe", "1234567890");

  // Uncomment the following line to read all contacts
 

  // Uncomment the following line to update a contact
  updateContact(1, "John Doe", "9876543210");

  // Uncomment the following line to delete a contact
  for (int i = 0; i < 255; i++) {
    deleteContact(i);
  } readContacts();
}

void loop() {
  // Main program loop


}
char hashing(String input) {
  unsigned char* hash = MD5::make_hash("hello world");
  char *md5str = MD5::make_digest(hash, 8);
  free(hash);
  Serial.println(md5str);
  return (md5str);
}

void createContact(const char* Name, const char* number) {
  Contact newContact;
  strncpy(newContact.Name, Name, sizeof(newContact.Name) - 1);
  strncpy(newContact.number, number, sizeof(newContact.number) - 1);

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
    Serial.println(contact.Name);
    Serial.print("Number: ");
    Serial.println(contact.number);
  }
}

void updateContact(int index, const char* Name, const char* number) {
  if (!isValidIndex(index)) {
    Serial.println("Error: Invalid contact index!");
    return;
  }

  int address = index * CONTACT_SIZE;
  Contact updatedContact;
  strncpy(updatedContact.Name, Name, sizeof(updatedContact.Name) - 1);
  strncpy(updatedContact.number, number, sizeof(updatedContact.number) - 1);

  EEPROM.put(address, updatedContact);

  Serial.println("Contact updated successfully!");
}

void deleteContact(int index) {
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
  return contact.Name[0] == '\0' && contact.number[0] == '\0';
}

bool isValidIndex(int index) {
  return index >= 0 && index < MAX_CONTACTS;
}
