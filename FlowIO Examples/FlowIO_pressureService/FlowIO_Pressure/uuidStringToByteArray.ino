void uuidStringToByteArray(String strUUID, uint8_t binUUID[]) {
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0 ; i--) {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}
char nibble2c(char c) {
  if ((c >= '0') && (c <= '9')) return c - '0';
  if ((c >= 'A') && (c <= 'F')) return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f')) return c + 10 - 'a';
  return 0;
}
char hex2c(char c1, char c2) {
  return (nibble2c(c1) << 4) + nibble2c(c2);
}
