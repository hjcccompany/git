#include "git-compat-util.h"
#include "./sha256.h"

#define BLKSIZE blk_SHA256_BLKSIZE

void blk_SHA256_Init(blk_SHA256_CTX *ctx)
{
	ctx->offset = 0;
	ctx->length = 0;
	ctx->state[0] = 0x6A09E667UL;
	ctx->state[1] = 0xBB67AE85UL;
	ctx->state[2] = 0x3C6EF372UL;
	ctx->state[3] = 0xA54FF53AUL;
	ctx->state[4] = 0x510E527FUL;
	ctx->state[5] = 0x9B05688CUL;
	ctx->state[6] = 0x1F83D9ABUL;
	ctx->state[7] = 0x5BE0CD19UL;
}

static inline uint32_t ror(uint32_t x, unsigned n)
{
	return (x >> n) | (x << (32 - n));
}

#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         ror((x),(n))
#define R(x, n)         ((x)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

static void blk_SHA256_Transform(blk_SHA256_CTX *ctx, const unsigned char *buf)
{

	uint32_t S[8], W[64], t0, t1;
	int i;

	/* copy state into S */
	for (i = 0; i < 8; i++) {
		S[i] = ctx->state[i];
	}

	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++, buf += sizeof(uint32_t)) {
		W[i] = get_be32(buf);
	}

	/* fill W[16..63] */
	for (i = 16; i < 64; i++) {
		W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
	}

#define RND(a,b,c,d,e,f,g,h,i,ki)                    \
	t0 = h + Sigma1(e) + Ch(e, f, g) + ki + W[i];   \
	t1 = Sigma0(a) + Maj(a, b, c);                  \
	d += t0;                                        \
	h  = t0 + t1;

	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],0,0x428a2f98);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],1,0x71374491);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],2,0xb5c0fbcf);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],3,0xe9b5dba5);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],4,0x3956c25b);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],5,0x59f111f1);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],6,0x923f82a4);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],7,0xab1c5ed5);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],8,0xd807aa98);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],9,0x12835b01);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],10,0x243185be);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],11,0x550c7dc3);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],12,0x72be5d74);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],13,0x80deb1fe);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],14,0x9bdc06a7);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],15,0xc19bf174);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],16,0xe49b69c1);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],17,0xefbe4786);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],18,0x0fc19dc6);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],19,0x240ca1cc);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],20,0x2de92c6f);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],21,0x4a7484aa);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],22,0x5cb0a9dc);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],23,0x76f988da);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],24,0x983e5152);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],25,0xa831c66d);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],26,0xb00327c8);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],27,0xbf597fc7);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],28,0xc6e00bf3);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],29,0xd5a79147);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],30,0x06ca6351);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],31,0x14292967);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],32,0x27b70a85);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],33,0x2e1b2138);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],34,0x4d2c6dfc);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],35,0x53380d13);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],36,0x650a7354);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],37,0x766a0abb);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],38,0x81c2c92e);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],39,0x92722c85);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],40,0xa2bfe8a1);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],41,0xa81a664b);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],42,0xc24b8b70);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],43,0xc76c51a3);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],44,0xd192e819);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],45,0xd6990624);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],46,0xf40e3585);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],47,0x106aa070);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],48,0x19a4c116);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],49,0x1e376c08);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],50,0x2748774c);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],51,0x34b0bcb5);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],52,0x391c0cb3);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],53,0x4ed8aa4a);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],54,0x5b9cca4f);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],55,0x682e6ff3);
	RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],56,0x748f82ee);
	RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],57,0x78a5636f);
	RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],58,0x84c87814);
	RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],59,0x8cc70208);
	RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],60,0x90befffa);
	RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],61,0xa4506ceb);
	RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],62,0xbef9a3f7);
	RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],63,0xc67178f2);

#undef RND

	for (i = 0; i < 8; i++) {
		ctx->state[i] = ctx->state[i] + S[i];
	}
}

#define MIN(x, y) ((x) < (y) ? (x) : (y))
void blk_SHA256_Update(blk_SHA256_CTX *ctx, const void *data, size_t len)
{
	const unsigned char *in = data;
	size_t n;
	ctx->length += len;
	while (len > 0) {
		if (!ctx->offset && len >= BLKSIZE) {
			blk_SHA256_Transform(ctx, in);
			in += BLKSIZE;
			len -= BLKSIZE;
		} else {
			n = MIN(len, (BLKSIZE - ctx->offset));
			memcpy(ctx->buf + ctx->offset, in, n);
			ctx->offset += n;
			in += n;
			len -= n;
			if (ctx->offset == BLKSIZE) {
				blk_SHA256_Transform(ctx, ctx->buf);
				ctx->offset = 0;
			}
		}
	}
}

void blk_SHA256_Final(unsigned char *digest, blk_SHA256_CTX *ctx)
{
	const unsigned trip = BLKSIZE - sizeof(ctx->length);
	int i;

	ctx->length <<= 3;
	ctx->buf[ctx->offset++] = 0x80;

	if (ctx->offset > trip) {
		memset(ctx->buf + ctx->offset, 0, BLKSIZE - ctx->offset);
		blk_SHA256_Transform(ctx, ctx->buf);
		ctx->offset = 0;
	}

	memset(ctx->buf + ctx->offset, 0, BLKSIZE - ctx->offset - sizeof(ctx->length));

	put_be64(ctx->buf + trip, ctx->length);
	blk_SHA256_Transform(ctx, ctx->buf);

	/* copy output */
	for (i = 0; i < 8; i++, digest += sizeof(uint32_t))
		put_be32(digest, ctx->state[i]);
}