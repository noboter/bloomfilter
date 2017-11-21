#include "BloomFilter.h"

BloomFilter::BloomFilter(uint64_t size, uint8_t numHashes): m_bits(size), m_numHashes(numHashes) {}

std::array<uint64_t, 2> hash(const uint8_t *data, std::size_t len) {
    std::array<uint64_t, 2> hashValue;
    MurmurHash3_x86_128(data, len, 0, hashValue.data());

    return hashValue;
}

inline uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize) {
    return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const uint8_t *data, std::size_t len) {
  std::array<uint64_t, 2> hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
  }
}

bool BloomFilter::possiblyContains(const uint8_t *data, std::size_t len) const {
  std::array<uint64_t, 2> hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) {
          return false;
      }
  }

  return true;
}

void BloomFilter::binary_write(std::ofstream& fout){
    std::vector<bool>::size_type n = m_bits.size();
    fout.write((const char*)&n, sizeof(std::vector<bool>::size_type));
    for(std::vector<bool>::size_type i = 0; i < n;){
        unsigned char aggr = 0;
        for(unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            if(m_bits.at(i))
                aggr |= mask;
        fout.write((const char*)&aggr, sizeof(unsigned char));
    }
}

void BloomFilter::binary_read(std::ifstream& fin){
    std::vector<bool>::size_type n;
    fin.read((char*)&n, sizeof(std::vector<bool>::size_type));
    m_bits.resize(n);
    for(std::vector<bool>::size_type i = 0; i < n;){
        unsigned char aggr;
        fin.read((char*)&aggr, sizeof(unsigned char));
        for(unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            m_bits.at(i) = aggr & mask;
    }
}
