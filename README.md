# SHA-256 Implementation in C++
A from-scratch implementation of the SHA-256 hashing algorithm in C++, written by following the original NIST specification. No cryptographic libraries used — just standard C++.

## Basic Flow of the Algorithm 
SHA-256 processes input in four stages:
**1. Preprocessing (padding)**  
The input string is padded to a multiple of 512 bits. A `1` bit is appended after the message, followed by zero bits, and finally the original message length as a 64-bit big-endian integer. This ensures the message fits cleanly into 512-bit blocks.

**2. Message Schedule**  
Each 512-bit block is parsed into 16 thirty-two-bit words (`W[0..15]`). These are then expanded into 64 words (`W[0..63]`) using the σ0 and σ1 mixing functions, which rotate and shift bits to spread the message's influence across all 64 rounds.

**3. Compression**  
Eight 32-bit working variables (`a` through `h`) are initialized with the current hash state. For each of the 64 rounds, two temporary values `T1` and `T2` are computed using the `Ch`, `Maj`, `E0`, and `E1` functions, and the working variables are updated. After all 64 rounds, the results are added back into the hash state.

**4. Final Hash**  
After processing all blocks, the eight hash values `H[0..7]` are concatenated as hex strings to produce the final 256-bit (64 character) digest.

## Reference

For any of you who are interested in learning how the algorithm works under the hood,below is the official FIPS paper which descibes it:
> FIPS PUB 180-4 — *Secure Hash Standard (SHS)*  
> National Institute of Standards and Technology (NIST), 2015  
> https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
> Also, I found this youtube video very helpful too in visualizing the algorithm step-by-step:
>https://youtu.be/orIgy2MjqrA?si=UgVyzMznixwETxtA