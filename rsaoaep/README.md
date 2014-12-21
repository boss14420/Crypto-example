# RSA-OAEP

## RSA
Trong bài này, ta sẽ cài đặt thuật toán khóa mã chung RSA với keysize = 6144 bit.

Các bước thực hiện như sau:

### Sinh khóa
Sinh hai số nguyên tố ngẫu nhiên `p`, `q` khác nhau, có kích thước 6144 / 2 = 3072 bit.

Tính các giá trị sau:

    n = p * q
    phi = (p - 1) * (q - 1)
    chọn e < phi sao cho gcd(e, phi) = 1
    d = e^-1 (mod phi)
    dp = d mod (p - 1)
    dq = d mod (q - 1)
    qinv = q^-1 mod p

Trong đó phép lấy nghịch đảo đồng dư được thực hiện bằng thuật toán Euclid mở rộng.
Sau khi tính được các giá trị trên, chia ra làm hai bộ:

1. `(n, e)` được sử dụng làm khóa công khai (public key),
2. `(n, e, d, p, q, dp, dq, qinv)` được sử dụng làm khóa bí mật (secret key).

### Mã hóa
Cho đầu vào là số nguyên `m < n`. Thủ tục mã hóa RSA được thực hiện bằng công thức:

    c = m^e (mod n)

Thủ tục này có thể đượng thực hiện khi có khóa công khai hoặc khóa bí mật.

### Giái mã
Để giải mã bản mã được biểu diễn bởi số nguyên `c < m`. Ta thực hiện các phép toán:

    m1 = c^dp mod p
    m2 = c^dq mod q
    h = qinv * (m1 - m2) mod p (h > 0)
    m = m2 + q * h

Kết quả trả về là `m`. Muốn thực hiện thủ tục này cần có khóa bí mật.

### Cài đặt
Trong cài đặt của chương trình (file `include/rsa.hpp`), có lớp `RSAPublicKey` biểu diễn cho khóa RSA công khai. Đối tượng của lớp này có thể thực hiện mã hóa một số nguyên `m < n` (phương thức `RSAPublicKey::encrypt_int()`).

Cũng trong file đó còn có lớp `RSAKey` là lớp kế thừa của lớp `RSAPublicKey`, biểu diễn cho khóa bí mật. Đối tượng của lớp `RSAKey` ngoài mã hóa còn có thể giải mã (phương thức `RSAKey::decrypt_int()`). Ngoài ra lớp `RSAKey` còn có phương thức `RSAKey::publickey()` trả về khóa công khai tương ứng.

## OAEP
Trong thực tế, người ta không sử dụng trực tiếp các thủ tục RSA kể trên để mã hóa các bản tin vì lý do bảo mật (đã có nhiều phương pháp tấn công). Thay vào đó, bản tin cần phải padding trước khi được mã hóa bằng RSA. Một trong số những phương pháp padding là OAEP (*Optimal Asymmetric Encryption Padding*), được quy định trong chuẩn PKCS#1 v2.2.

Ta quy định một số ký hiệu sau:

- `Hash`: hàm băm an toàn, có kích thước dầu ra là `hLen`. Ở đây ta sử dụng hàm `SHA3-256`, có `hLen = 32 byte`.
- `MGF` (*Mask generation function*): hàm đơn định sinh ra một chuỗi kích thước độ dài tùy ý từ một chuỗi `seed`.
- `k`: kích thước giá trị `n` trong khóa RSA, tính bằng byte. Với RSA6144 thì `k = 768`.

### Mã hóa RSA-OAEP
Đầu vào: chuỗi byte `M` kích thước `mLen`. Yêu cầu `mLen <= k - 2hLen -2`, với RSA6144 và hàm hash SHA3-256 thì giá trị tối đa của mLen là 702.

1. Kiểm tra kích thước: nếu `mLen > k - 2hLen - 2` thì báo lỗi “message too long” và dừng thuật toán,
2. OAEP Padding:

    a. đặt `lHash = hash(‘’)` (hash của chuõi rỗng).

    b. sinh chuỗi `PS` bao gồm `k - mLen - 2hLen - 2` byte giá trị 0. PS có thể rỗng,

    c. đặt `DB = lHash || PS || 0x01 || M`. Như vậy DB có kích thước `k - hLen - 1`

    d. sinh chuỗi ngẫu nhiên `seed` kích thước `hLen`,

    e. đặt `dbMask = MGF(seed, k - hLen - 1)`

    f. `maskedDB = DB XOR dbMask`

    g. `seedMask = MGF(maskedDB, hLen)`

    h. `maskedSeed = seed XOR seedMask`

    i. đặt `EM = 0x00 || maskedSeed || maskedDB`. Như vậy EM có kích thước bằng `k`.

3. Mã hóa RSA

    a. Chuyển chuỗi `EM` thành số nguyên `m` theo kiểu big-endian (byte có ý nghĩa nhất đứng đầu chuỗi). Vì `EM` có byte đầu tiên là 0x00 nên giá trị của `m` luôn nhỏ hơn modulo của khóa RSA `n`,

    b. Áp dụng thủ tục mã hóa RSA: `c = RSAEP(publickey, m)`

    d. Chuyển số nguyên `c` thành chuỗi `C` kích thước `k` theo kiểu big-endian.

4. Kết quả trả về là C.

Thủ tục padding của OAEP được biểu diễn bởi hình dưới đây:
![oaep](oaep.png)

### Giải mã RSA-OAEP
Đầu vào: chuỗi byte `C` kích thước `k`.

1. Kiểm tra kích thước: nếu kích thước `C` khác `k` thì báo lỗi “decryption error” và kết thúc,
2. Giải mã RSA

    a. Chuyển chuỗi byte C thành số nguyên `c`

    b. Giải mã RSA: `m = RSADP(secretkey, c)`. Nếu quá trình giải mã báo lỗi (tức là `c >= n`) thì báo lỗi “decryption error” và kết thúc,

    c. Chuyển số nguyên m thành chuỗi `EM` kích thước k

3. OAEP decoding

    a. đặt `lHash = hash(‘’)`

    b. Chia EM thành byte `Y`, chuỗi `maskedSeed` kích thước `hLen`, chuỗi byte `maskedDB` kích thước `k - hLen - 1`: `DB = Y || maskedSeed || maskedDB`,

    c. đặt `seedMask = MGF(maskedDB, hLen)`

    d. `seed = maskedSeed XOR seedMask`

    e. `dbMask = MGF(seed, k - hLen - 1)`

    f. `DB = maskedDB XOR dbMask`

    g. Chia DB thành các chuỗi: `lHash2` kích thước `hLen`, `PS` bao gồm các byte 0 (PS có thể rỗng), và M sao cho: `DB = lHash2 || PS || 0x01 || M`.

    Nếu không có byte 0x01 nào phân chia PS và M, hoặc nếu `lHash2` khác `lHash`, hoặc nếu Y khác 0 thì báo lỗi “decryption error” và kết thúc,

4. Giá trị trả về là chuỗi M

### Cài đặt
Cài đặt OAEP nằm trong file `include/oaep.hpp`, trong đó có lớp `OAEP`. Mỗi đối tượng `OAEP` có một khóa RSA (public key hoặc secretkey) tương ứng, tùy vào loại khóa mà có thể giải mã hay không. Việc mã hóa được thực hiện bởi phương thức `OAEP::encrypt()`, còn giải mã bởi phương thức `OAEP::decrypt()`.

Trong cài đặt `OAEP::decrypt()`, ở bước `3.g` cần chú ý để tránh timing attack. Tức là cho dù có trả về lỗi nào trong 3 loại lỗi kể trên thì thời gian thực hiện vẫn phải không được chênh lệch nhau quá nhiều.

Đoạn mã kiểm tra ở bước 3.g như sau:

    bool error = false;
    if (!std::equal(begin(lHash), end(lHash), lHash2)) error = true;
    if (*Y != 0) error = true;

    auto p = PS;
    byte *sep = nullptr;
    while (p != PS + k - 2*mgf.hLen() - 1) {
        if (*p != 0) {
            sep = sep ? sep : p;
        }
        ++p;
    }
    if(*sep != 1) error = true;

    if (error) throw "decryption error";

Ngoài việc luôn phải kiểm tra hết cả ba loại lỗi, đoạn mã trên có yêu cầu kiểm tra kết thúc chuỗi PS từ đầu cho đến hết chuỗi DB, như vậy kẻ tấn công cũng không thể lấy được thông tin về độ dài của PS (và theo đó là độ dài của M) dựa vào thời gian thực hiện.

## Chương trình minh họa
Để biên dịch các chương trình dưới đây, chỉ cần chạy lệnh `make`. Đề biên dịch thành công thì yêu cầu máy cài đặt sẵn thư viện tính toán số lớn `GMPLib` (https://gmplib.org/).
Ngoài ra, vì sử dụng `/dev/urandom` để sinh các chuỗi ngẫu nhiên nên hiện tại chỉ có thể chạy trên các hệ điều hành *nix;

### Chương trình sinh khóa
Chương trình `rsa_genkey` sinh ra ngẫu nhiên một cặp khóa công khai và khóa bí mật với độ dài lớn tùy ý. Cú pháp như sau:

    ./rsa_genkey keysize keyname

Với `keysize` là kích thước modulo n (tính bằng bit), n phải chia hết cho 16. Ví dụ, khi chạy lệnh:

    ./rsa_genkey 6144 abc

Sẽ sinh ra hai file là `abc.publickey` và `abc.secretkey`. 
Nội dung của file `publickey` bao gồm:

- keysize (theo bit), biểu diễn dưới dạng thập phân
- modulo `n`, biểu diễn dưới dạng hexa
- exponent `e`, biểu diễn dưới dạng hexa

Ví dụ:

    6144
    612da9b1484b7e94539e1d204374323ba7d5acb7849dada534f32e14403c725c9b792574ddd5ca300c0f52ac35808877851665162c92f2c9c68c1454731c80397e543c830f50c5ff8e27767fc1ff213f4d56774feff79de0ecbb0f3918323a04c2b4da77dd231dcece7edb68e56200b4417f8634c641110fa541d024cc15b23b79509d623033b268ca3bd96bfb5f0357de1dc8d8f6ae1d2c73e1ced124a6b0258dec869fb315123f5ce17e8b86172d4d59ff2bda32c4096a26494ded416272548d775584a99c78cbba317207fe9cf2626fe42e65ead0463fa59bfaa3127e48f26b5c61e9123a6acefbd70c429c02461df7d07c17ded4d0ba97a4c9edd1ea8a6ce98afe8fac316efe65a362dadc471464951653eef5ed75fe9ed235c3ff63066ed1031f4fcb54e523ebe28beaf3723d67174e7d40b20713b4dca5f3c21fed2759401f73cb63d189955e5c9ce81bdf80009534b53b000bc0562aa21d2c0fccb85804a7e471bb1e426cdab87a3ef3add71500020c5099e9f1ca7b60d545d9e82f6acfb32ce058570c47eb2d27206eb57574b5f21d7d43e9838694d073085fdb84cbcebc2a98a86a93062a52b0a2c5fe345575567349f19a646d432548502abebb9590ce7e372401aaed0f95ec82b2ce90d12f7927bb5466c70294ed6914fca14dc19fecff0cb8b360a88c85b538ca2de5c033a2c750e614db8de2dcfa1dd9e4f7cce9f6e59675a3a94228c38a712c867d1c346c0f672c3837c9f43ae4579f07e5e4e94d5f8795189540a3e0f33ae932178f6b5a8f1defbfe79ee8b28bfa2d72d44bae417764f26b358308aa7dd8a4b363154cc8c09993baf7aad230b9128976ffab3f13f0e235a02a78f6fd38f311a447ccf6f715ee722a9fa4aa2ab5e06238917616796d81e34c012a9d9972ec539e126bf45745b55cb59caecbe037cc57b4738ff1cc565584e877a028fce1735fa4b8b950df32ca435cd1aa5eea9376702f232580d627023e8e86c8fed408592ca7b15b4088cfd4035d68c60063cd40dabd14d129c8be2edce615689edfae5e14389c3025354470d2a8f139b5ceb367331bd09b
    10001

File `secretkey` có 3 dòng đầu giống với file `publickey`, và có thểm các dòng tiếp theo như sau:

- d (hexa)
- p (hexa)
- q (hexa)
- dp (hexa)
- dq (hexa)
- qinv (hexa)

Ví dụ:

    6144
    612da9b1484b7e94539e1d204374323ba7d5acb7849dada534f32e14403c725c9b792574ddd5ca300c0f52ac35808877851665162c92f2c9c68c1454731c80397e543c830f50c5ff8e27767fc1ff213f4d56774feff79de0ecbb0f3918323a04c2b4da77dd231dcece7edb68e56200b4417f8634c641110fa541d024cc15b23b79509d623033b268ca3bd96bfb5f0357de1dc8d8f6ae1d2c73e1ced124a6b0258dec869fb315123f5ce17e8b86172d4d59ff2bda32c4096a26494ded416272548d775584a99c78cbba317207fe9cf2626fe42e65ead0463fa59bfaa3127e48f26b5c61e9123a6acefbd70c429c02461df7d07c17ded4d0ba97a4c9edd1ea8a6ce98afe8fac316efe65a362dadc471464951653eef5ed75fe9ed235c3ff63066ed1031f4fcb54e523ebe28beaf3723d67174e7d40b20713b4dca5f3c21fed2759401f73cb63d189955e5c9ce81bdf80009534b53b000bc0562aa21d2c0fccb85804a7e471bb1e426cdab87a3ef3add71500020c5099e9f1ca7b60d545d9e82f6acfb32ce058570c47eb2d27206eb57574b5f21d7d43e9838694d073085fdb84cbcebc2a98a86a93062a52b0a2c5fe345575567349f19a646d432548502abebb9590ce7e372401aaed0f95ec82b2ce90d12f7927bb5466c70294ed6914fca14dc19fecff0cb8b360a88c85b538ca2de5c033a2c750e614db8de2dcfa1dd9e4f7cce9f6e59675a3a94228c38a712c867d1c346c0f672c3837c9f43ae4579f07e5e4e94d5f8795189540a3e0f33ae932178f6b5a8f1defbfe79ee8b28bfa2d72d44bae417764f26b358308aa7dd8a4b363154cc8c09993baf7aad230b9128976ffab3f13f0e235a02a78f6fd38f311a447ccf6f715ee722a9fa4aa2ab5e06238917616796d81e34c012a9d9972ec539e126bf45745b55cb59caecbe037cc57b4738ff1cc565584e877a028fce1735fa4b8b950df32ca435cd1aa5eea9376702f232580d627023e8e86c8fed408592ca7b15b4088cfd4035d68c60063cd40dabd14d129c8be2edce615689edfae5e14389c3025354470d2a8f139b5ceb367331bd09b
    10001
    2da52422b8617588382a7965d1293ffb135f632c2427680bc46e410394149066fd440f1af0a3ddbf69babccb54aa15ef85dcba088b514ea706127fee16cf994ee80eab09a864f94b8b3af0071c593905a1ceee65e6c2392f1022e108a15bde32a43e0cabe67bfb214b8efc822dd4359e75716fd95411f5f0925ff2638654de8ddef2c5a9fc181c41ea53e85137162edcd3e0cb99556cfd190729565a984b8f4b647967d695685d54d3ccbb02d02e0cc25b7666e3ab9ab70d5c046c8cf77c45557fc940aafa7c1ba51dc4e1d9ed774cc67de6010130253a09e540f99e252e9a16fbe4bdbc9b2a43470bb5bbf5818ff3dd59ac5ccd5d863ab09a4ff5e536076393bab2e5d5183d73f41c4b7d406ddd19301173166394b2400d240e7ae0a011a450e2ee0384186ebcde7d8ac0cbd023a5c1c989bd7d68e1b818e8d72e42cd7a38544cb139539c842c8ab372eee47b4b24b5a15fcb8c75fe0f0eaa21d17003cfd5fac9dd0e897929c26b9ce4afcee7e88cb1397abccc132f76b9f3efccdd6d7808e51d4874753e8991e4a978f3701777858af29e5582c764745aaf4dc8662b61bd9bbcebae0ebb2b883c110179e58e132892b6421d70504b9228ace914b800d85c0516c690e36f36bad0a1f9186e90b78a03727e8ec6b03ab2108029f3ffb0d7d3d02e5f2a4000e8325b215c0243bd1ed379a0029fed415a1c54f9d0737093667c0c9b072b3fc00e3193754581a0871fd658fbd1751012408e6d2d078aaf825f705bbb108eba9eaa87a82747a14d57f5d18fb960d194f431d60f6978a0ef1eb2372fca8db60fbb8d3e9446a5f34fd63b8470cf070a9b2946aa14d2bb3064f26c6ffe681c75a7ba1498d1822587d363b530b241393ee2b7a08a2de491bc91d3a964faa13c923bfb129bf147b0eceb427184bc4c629d743fdab0939f0faf4dad915046975681b3fb723c561984eb16716ad8372c179a9e117be937b2a932ddae2d3d2e52119a64f6c982f5090e7466f023a75dfd564e983efb2e20e5a0318c75ab5f29aa1e5086414ec3ab7f5e99f0a7c38184f0d21ff1689867bfa9aee2f551b66439
    bc09a62c18a0a9896ae5662f6896169390c6a77d152695b2bd7f071b6fb1fd86693b399dca7719d8282cd47f86a9e04a79ba3e4722ec1c30d6b648a0dc4a118534ba6ac9b9bf3cf0ea4e469d7ff461dcb887088a6f55f9b618d97b33712dc55093144b847cc8298fcb604c44abb09d810e549c622e226d9b9cd554b940f5797b510e8a33327c26074e96a75728589b12655dea266343a15d6ac95119368b6e8ffbc13aa7f75d68cafea98ecc6bf37ad8784dfc54bbaf106e489dcd33cc41c5aab4b39b5e09404df9e6e2702b22ef4ef3cda35ca5f81dc23bec949cd30eb95b205b3b0bb45c913601429b8902a19224a8514782153a5d7dc957bd22c238f28247191d85d8fade72b0166fa91698da26757105fb37c20eedb6f919a2540a7cc00ac296cbb18457460835c3edd6e78957ef05a36ceaa2e4dff585fef2809830ad61f70ebc21101beed58d5d0c5c2daa5aff45d4be491f2cf97aa6d5fc74ef0eef92e2bd4a70fb79e5c38dddb26d8c2d1036bd366a23ea193603df8ba9f5b5c9734d
    844d2d12e13db6831849d780d24c783fa00618467b20bd1e2ee2739e55a13fe7b58f9ea7583f8177507b939c75169535d8aaa7ac18815ed88fa590a0e5e241e1d4f11793c4d6cab33af22fc3f0cb92eef03af85e59fd43bab0aa8d63e538a1b2b4a644269dc437cf26433a1a0e04b978132e9dc3c11bc5b735cafe4b3368eeabc0e0a75fcefc168c1ec4af5b825369101a549e601175b7954cabaef1e3bbdf26982b8d5b9567c23ae43285bf302f4187cbeee27531b6ca308566ee7eede45727af1e885c8e18a1ea7057ece5921abecb781c9a69c87d7cca86f86fc6898ea4dc1f6053e8a67c03f5bf1595d18590489670840636dae1a22c5dbfe3571bf2fb90b443fbbf51575a1a872517cc676be2558f070fabb4fd4099a2dcf39d9164fe35a475a4a21cf9483606f0b1d9f49da9f92b91245743936c731bb153c727e17dcf2fee37a7aa1f9524d11d529d9cb6122913ec98473df178a6461394dd3114bc8a808f6f368273569ab87d0bbd05da38777eac6838ba8a6c325fb12e3fdc588f87
    404aec4e577d577a6cda52be12fc7e6ba62cc647b65ba303a257348ee4f9eebd6e6d999da2046709c972ca956fedd58aa16454b6edf1dd11b5d84dacf2a540a853070a4c3efe6c4fd34d4047c9e03d35c2c228e16c7ff1099a2582426c8841d6f2f3157f0f4b412ab15713bcc7177443142b49ed27e90da10f56c1c935b4b8fcb912479e89bc79243672bc74c88ec336df01e5758aab2d194bd1d9b0cd3d8f681ba44df1f05a37e27080791adc2e3bea97f7d23706952d0108432c4e71a06b7d9a9b6e9c107db42ba60df65c496d998932de90670f16ddbaaf8180bd8963470d212467341c126a6cb3e18a4f23ff5c1950f12a4127db9f8ea0394311357775a3e9aa768162057d21480a95bdec356ac0ed0c26814210e8a68f357c934fa2a7e705f19963f0e40645f85c2d8e354c788c79590f7c8a85c72da3cb300f57fa0cce5a32e7904679c687adf60aabd9d14d9bcabc9af3557d832fb4aba5ce358eddcb534b75907beb89ebfb2dba3c4a997dd9b98849724e30a08f6277d67cf4d31461
    4c82852a81efba789db736555d48b3d1b7ad4ea493d81f06317eebf608f73389c049ca2a64b2752096fea022bee3cc6e535a7d94dca7143e90f3e26bea05aaabfb0561e926d04070de09e8ed9b775f05a70fb34a7d2825def503ecb8747b07a7dd28ba08cafc0a7a555a536375efed822f35aa2ee6a234f8fab6e5d07f6e9377e817033438f337e6d8744e24d17d8328b8d16eb587e357c3938f45a0c55e847241560e6d843d78e9adc639ea92e701329311170d472eba46cd1ef59599e58f3177e8645b822164a1bb4e5dc57574da19b8e071bbf321fee4f0a103a30131e8cd13a8155e59c926511bac53f3d315d9a486751352edc9c68b79bbdab1298c9f46d3b8ed017afcd532ba2d0d523c0cfbdb16d7b28c4e22971542461cec6684cd2e20699847d811a15cfd2e7cebdab6c2f141ecb1e37544f056b1111fadf0b64a9762d49482f5b34601009dc0853d775d913be07b3fd7e6a92b1f237bd75ee6eb11fdbef545c4b306c67b2275835ebb1024aab0f1a4bcdc082e2980c18a54d73249
    5dbd75a4953c5823c7c3d725331bfcb755b2b5dd73dc1e70e8a58d5877c6daca35e39e89bfc5ff76185bea8729930857804463a0c4fb4cff6e17059185987cf47fb26de7a5e1343b1a522455a4e5a523d8b4854d97497f6b03180209febf34c285391ac17c69656ef304e06720a5ed9097f86b2b524c9532e348ca645bc8be895cd4dc288d1ac66b600524ac1cddeb97b303b288c8c501b4c92fe399ca297c6d0418bd9b0d20e0f01678ce6e2fa4e9585857e247c7881eac96ffbc854815fa34db6725f6733c2b6214e71103caed26d52f08ed449ab1253b0f788f18fb3a8584002cd0cbf0475f7479c00570a1a49aed3ff8490a418d52978748454f00c32ca497213ef076877b184e6c090079171d7efcb3d9442caa062f2ca5253c7d7659f655a44152d4d3615e208a3419014afed481ae26364e99ab7300bd365b0ae638f882af24535a54b5d5793973983ae5723fb70c2e4fe4fab04449a2a5455c4ec2a5bd36e1a683b5316844171b860478a6615c7af6fc4387cff599660e215cefb3b4

Với keysize = 6144 thì thời gian sinh khóa là khoảng từ 5s đến 1phút.

### Chương trình mã hóa / giải mã bằng RSA-OAEP
Chương trình `oaeptest` nhận tham số là khóa (bí mật hoặc công khai) RSA. Chuỗi đầu vào được nhập từ `stdin`, và kết quả mã hóa hoặc giải mã được ghi vào `stdout`.

Để mã hóa một file `filename` bằng khóa đã được sinh ra ở trên, ta dùng lệnh:

    cat filename | ./oaeptest e abc.publickey > filename.enc

Kết quả mã hóa được ghi trong file `filename.enc`, kích thước của file này lớn hơn file gốc.

Để giải mã, ta cần khóa bí mật:

    cat filename.enc | ./oaeptest d abc.secretkey > filename.dec

#### Kết quả chạy thử

Các kết quả sau được chạy trên máy tính Linux 64bit 3.17.4, CPU Intel Core 2 Duo SU9400 1400Hz x 2, GMPLib bản 6.0.0.


| File            | Dung lượng | File mã  | Tỉ lệ     | Thời gian     | Giải mã  | K/t MD5 |
|-----------------|------------|----------|-----------|---------------|----------|---------|
| abc.png         | 63KiB      | 69KiB    | 1.097     | 0.14s         | 7.10s    | OK      |
| bigint.o        | 3.8KiB     | 4.5KiB   | 1.215     | 0.03s         | 0.48s    | OK      |
| oaeptest.o      | 38KiB      | 42KiB    | 1.100     | 0.10s         | 4.21s    | OK      |
| rsa_genkey.o    | 36KiB      | 39KiB    | 1.113     | 0.09s         | 3.99s    | OK      |
| rsatest.o       | 37KiB      | 40KiB    | 1.100     | 0.10s         | 4.10s    | OK      |
| util.o          | 1.8KiB     | 2.3KiB   | 1.286     | 0.03s         | 0.25s    | OK      |

Ta thấy thời gian giải mã lớn hơn hẳn so với thời giản mã hóa, đó là do trong quá trình mã hóa, thừa số mũ `e` có giả trị nhỏ (65537). Còn trong quá trình giải mã, hai thừa số mũ `dp`, `dq` có giá trị lớn (cỡ `2^3072`).
