# SBTG
PoC Metamorphic Crypter Engine for ELF32 files

Encapsulates a given bynary in a series of arbitrary XOR and RC4 layers all using different keys.
Each build will contain different instructions to implement the aforementioned schemes. The payload has also polymorphic attributes, as the same schemes to decrypt the embedded blob are also used for decrypting some portions of the code.
There is potential to make the implemented anti-forensic capabilities of this project stronger implementing some sort of anti-debugging layer in between of each decryption layer, but as the title states is just a PoC :)
