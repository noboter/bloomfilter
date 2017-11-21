#include <vector>
#include <array>
#include <fstream>
#include "MurmurHash3.h"

//template< class Key, class Hash = std::hash<Key> >
struct BloomFilter {
    BloomFilter(uint64_t size, uint8_t numHashes);

    void add(const uint8_t *data, std::size_t len);
    bool possiblyContains(const uint8_t *data, std::size_t len) const;
    void binary_write(std::ofstream& fout);
    void binary_read(std::ifstream& fin);

    private:
        uint8_t m_numHashes;
        std::vector<bool> m_bits;
};
