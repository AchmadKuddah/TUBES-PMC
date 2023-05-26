#include <MD5.h>

void setup()
{
  //initialize serial
  Serial.begin(9600);
  //give it a second
  delay(1000);
  //generate the MD5 hash for our string
  unsigned char* hash=MD5::make_hash("AchmadNovel");
  //generate the digest (hex encoding) of our hash
  char *md5str = MD5::make_digest(hash, 16);
  free(hash);
  //print it on our serial monitor
  Serial.println(md5str);
  //Give the Memory back to the System if you run the md5 Hash generation in a loop
  free(md5str);
}

void loop()
{
}
