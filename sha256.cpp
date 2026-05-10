#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdint>
using namespace std;
#define mod512 512
/*Function:Integer to Hex auxiliary function
 @input:32-bit unsigned integer
 @output:hex string for the integer
*/
string integer_to_hex(uint32_t num){
    string hexSymbols="0123456789abcdef";
    string hex="";
    while(num){
        hex+=hexSymbols[num%16];
        num/=16;
    }
    reverse(hex.begin(),hex.end());
    while(hex.size()<8)hex="0" + hex; //Pad the front with 0's to ensure 4-byte hex;
    return hex;
} 


//Constant K values which are first 32-bits of fractional parts of cube roots of first 64 prime numbers
static const uint32_t K[64]={
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6,0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,0xc6e00bf3, 0xd5a79147, 0x06ca6351,0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


//Constant H values which are first 32-bits of fractional parts of square roots of first 8 prime numbers
uint32_t H[8]={
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
}; 


/*Function:x "chooses" between y and z bit by bit, x has 1--> choose next bit from y,x has 0--> choose next bit from z
 @input:x,y,z
 @output:32-bit integer which has mix of bits from y and z
*/
uint32_t ch(uint32_t x,uint32_t y,uint32_t z){
    return (x & y) ^ ((~x) & z);
}


/*Function:Finds out the bit which appears atleast twice at each place
 @input:x,y,z
 @output:32-bit unsigned integer with majority bits at each place
*/
uint32_t maj(uint32_t x,uint32_t y,uint32_t z){
    return (x & y)^(y & z)^(z & x);
}


/*Function:Simple right shift x by n places
 @input:x,n
 @output: (32-bit unsigned integer)x>>n or (x/pow(2,n));
*/
uint32_t shr(uint32_t x,uint32_t n){
    return x>>n;
}


/*Function:Rotate x by n places to the right
 @input:x,n
 @output:32-bit unsigned right rotated integer 
*/
uint32_t rotr(uint32_t x,uint32_t n){
    return shr(x,n) | (x<<(32-n));
}


/*Function:Mixing Functions E0,E1,sigma0,sigma1 to create a mixing effect from all elementary functions
 @input:x
 @output:32-bit unsigned integer 
*/
uint32_t E0(uint32_t x){
    return rotr(x,2) ^ rotr(x,13) ^ rotr(x,22);
}
uint32_t E1(uint32_t x){
    return rotr(x,6) ^ rotr(x,11) ^ rotr(x,25);
}
uint32_t sigma0(uint32_t x){
    return rotr(x,7) ^ rotr(x,18) ^ shr(x,3);
}

uint32_t sigma1(uint32_t x){
    return rotr(x,17) ^ rotr(x,19) ^ shr(x,10);
}


/*Function:Pad the input to 512 bit blocks usign modular arithmetic
 @input:input string
 @output:padded meesage as a byte-vector with message_size()%512=0  
*/
vector<uint8_t> pad_to_512_bits(string input){
        uint64_t messageBitLength=input.length()*8;
        int64_t k=(447-(int)(messageBitLength % mod512) +mod512)%mod512;
        vector<uint8_t> paddedMessage;
        for(char c:input){
            paddedMessage.push_back(c);
        }
        //Pad with 1 bit to indicate message termination
        paddedMessage.push_back(0x80);
        k=k-7;
        for(int i=0;i<k/8;i++){
            paddedMessage.push_back(0x00);
        }
        for(int i=7;i>=0;i--){
            paddedMessage.push_back((messageBitLength>>(i*8)) & 0xFF);
        }
        return paddedMessage;

}


/*Function:Build message Schedule W[i]
 @input:padded message, reference to W[i],current (512-bit)block number
 @output:void
*/
void buildMessageSchedule(vector<uint8_t> &paddedMesaage,uint32_t(&W)[64],int block){
    //Build message schedule W[0]->W[15]
        for(int i=0;i<16;i++){
            W[i]=((uint32_t)paddedMesaage[block*64 + i*4 + 0]<<24)|
                ((uint32_t)paddedMesaage[block*64 + i*4 + 1]<<16)|
                ((uint32_t)paddedMesaage[block*64 + i*4 + 2]<<8)|
                ((uint32_t)paddedMesaage[block*64 + i*4 + 3]);
        }
        for(int t=16;t<64;t++){
            W[t]=sigma1(W[t-2]) + W[t-7] + sigma0(W[t-15]) + W[t-16];
        }
}

/*Function:Hash computation usign H[i] and mixing functions
 @input:padded message
 @output:void,generate the final H[i]'s for  
*/
void compute_hash(vector<uint8_t> paddedMesaage){
    for(int block=0;block<paddedMesaage.size()/64;block++){
        uint32_t W[64];
        buildMessageSchedule(paddedMesaage,W,block);
        uint32_t a=H[0];
        uint32_t b=H[1];
        uint32_t c=H[2];
        uint32_t d=H[3];
        uint32_t e=H[4];
        uint32_t f=H[5];
        uint32_t g=H[6];
        uint32_t h=H[7];
        for(int t=0;t<64;t++){
            uint32_t T1 = h + E1(e) + ch(e,f,g) + K[t] + W[t];
            uint32_t T2 = E0(a) + maj(a,b,c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2; 
        }
        H[0] = a + H[0];
        H[1] = b + H[1];
        H[2] = c + H[2];
        H[3] = d + H[3];
        H[4] = e + H[4];
        H[5] = f + H[5];
        H[6] = g + H[6];
        H[7] = h + H[7];

    }
}

/*Function:Concatenate all H[i]'s to produce final MD
 @input:null
 @output:Final Hash for the string
*/
string buildMessageDigest(){
    string finalHash="";
    for(int i=0;i<8;i++){
        finalHash+=integer_to_hex(H[i]);
    }
    return finalHash;
}

/*Function:Wrapper for SHA-256,pads message,builds message schedule,computes final H[i]'s and concatenates them to generate final message digest
 @input:input string
 @output:SHA-256 hash for the string
*/
string sha256(string input){
    //Reset the hashes to their initial value for future computation
    H[0]=0x6a09e667; H[1]=0xbb67ae85; H[2]=0x3c6ef372; H[3]=0xa54ff53a;
    H[4]=0x510e527f; H[5]=0x9b05688c; H[6]=0x1f83d9ab; H[7]=0x5be0cd19;
    vector<uint8_t> paddedMessage = pad_to_512_bits(input);
    compute_hash(paddedMessage);
    string finalHash=buildMessageDigest();
    return finalHash;
}
int main(){
    string s;
    cout<<"Enter the string to be hashed: ";
    getline(cin,s);
    cout<<sha256(s)<<endl;
    return 0;
}
