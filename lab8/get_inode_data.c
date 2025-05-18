#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <endian.h>
#include <stdint.h>
#include <string.h>

#define SUPERBLOCK_OFFSET 1024
#define EXT2_SUPER_MAGIC 0xEF53
#define DESC_SIZE 32

struct ext2_super_block {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;

    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t  s_uuid[16];
    char     s_volume_name[16];
    char     s_last_mounted[64];
    uint32_t s_algorithm_usage_bitmap;

    uint8_t  s_prealloc_blocks;
    uint8_t  s_prealloc_dir_blocks;
    uint16_t s_padding1;

    uint8_t  s_journal_uuid[16];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;

    uint32_t s_hash_seed[4];
    uint8_t  s_def_hash_version;
    uint8_t  s_jnl_backup_type;
    uint16_t s_desc_size;

    uint32_t s_default_mount_opts;
    uint32_t s_first_meta_bg;
    uint32_t s_mkfs_time;

    uint32_t s_jnl_blocks[17];
    uint32_t s_reserved[172];
} __attribute__((packed));

struct ext2_group_desc {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
} __attribute__((packed));

struct ext2_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint32_t i_osd2;
} ;

int fill_with_zeros(uint32_t bytes_left, uint32_t block_size, char* buf){
    uint64_t bytes;

    while(bytes_left > 0){
        if(bytes_left > block_size) bytes = block_size;
        else bytes = bytes_left;

        memset(buf, 0, bytes);
        if (fwrite(buf, 1, bytes, stdout) != bytes) {
            return -1;
        }

        bytes_left -= bytes;
    }

    return 0;
}

int read_block(FILE *f, uint32_t block, uint32_t block_size, int level, uint64_t bytes_left, char* buf)
{
    if(bytes_left == 0) return 0;

    uint64_t bytes;
    if(bytes_left > block_size) bytes = block_size;
    else bytes = bytes_left;

    if(block == 0) {
        memset(buf, 0, bytes);
        if (fwrite(buf, 1, bytes, stdout) != bytes)
            return -1;
        
        return bytes;
    }

    if(level == 0) {
        if (fseek(f, (off_t)block * block_size, SEEK_SET))
            return -1;
        if (fread(buf, 1, bytes, f) != bytes)
            return -1;
        if (fwrite(buf, 1, bytes, stdout) != bytes)
            return -1;

        return bytes;
    }

    uint32_t *ptrs = (uint32_t *)malloc(block_size);
    if(!ptrs)
        return -1;
    uint32_t ptr_count = block_size / sizeof(uint32_t);

    if (fseek(f, (off_t)block * block_size, SEEK_SET)|| fread(ptrs, sizeof(uint32_t), ptr_count, f) != ptr_count) {
        free(ptrs);
        return -1;
    }

    int total_read = 0;

    for (uint32_t i = 0; i < ptr_count && bytes_left > 0; ++i) {
        uint32_t ptr = le32toh(ptrs[i]);
        int res = read_block(f, ptr, block_size, level - 1, bytes_left, buf);
        if (res < 0) {
          free(ptrs);
          return -1;
        }
        bytes_left -= res;
        total_read += res;
    }

    free(ptrs);
    return total_read;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "должно быть 3 аргумента\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");

    if(!f) {
        fprintf(stderr, "Ошибка fopen\n");
        return 1;
    }

    char *endptr;
    uint32_t inode_num = strtoul(argv[2], &endptr, 10);
    if (*endptr != '\0' || inode_num < 1) {
        perror("неверный номер inode\n");
        fclose(f);
        return 1;
    }

    struct ext2_super_block sb;

    if(fseek(f, SUPERBLOCK_OFFSET, SEEK_SET)) {
        perror("Ошибка fseek\n");
        fclose(f);
        return 1;
    }

    if(fread(&sb, sizeof(sb), 1, f) != 1) {
        perror("Ошибка fread\n");
        fclose(f);
        return 1;
    }

    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        perror("Это не ext2\n");
        fclose(f);
        return 1;
    }

    uint32_t block_size = 1024 << le32toh(sb.s_log_block_size);
    uint32_t inodes_per_group = le32toh(sb.s_inodes_per_group);
    uint32_t inode_size = le32toh(sb.s_rev_level) == 0 ? 128 : le16toh(sb.s_inode_size);
    uint32_t inode_count = le32toh(sb.s_inodes_count);

    if (inode_num > inode_count) {
        perror("Слишком много inode\n");
        fclose(f);
        return 1;
    }

    uint32_t desc_per_block = block_size / DESC_SIZE;
    uint32_t inode_group = (inode_num - 1) / inodes_per_group;
    uint32_t inode_index = (inode_num - 1) % inodes_per_group;
    uint32_t desc_block = 1024 / block_size + inode_group / desc_per_block + 1;
    uint32_t desc_offset = (inode_group % desc_per_block) * DESC_SIZE;

    struct ext2_group_desc group_desc;
    if (fseek(f, desc_block * block_size + desc_offset, SEEK_SET) || fread(&group_desc, sizeof(group_desc), 1, f) != 1) {
        perror("Ошибка fseek/fread\n");
        fclose(f);
        return 1;
    }
    
    uint32_t inode_table = le32toh(group_desc.bg_inode_table);
    off_t inode_offset = inode_table * block_size + inode_index * inode_size;

    struct ext2_inode inode;
    if (fseek(f, inode_offset, SEEK_SET) || fread(&inode, sizeof(inode), 1, f) != 1) {
        perror("Ошибка fseek/fread\n");
        fclose(f);
        return 1;
    }

    uint64_t file_size = ((uint64_t)le32toh(inode.i_dir_acl) << DESC_SIZE) | (uint64_t)le32toh(inode.i_size);
    //uint64_t file_size = le32toh(inode.i_size);
    char* buf = (char*)malloc(block_size);
    if(!buf){
        perror("Ошибка malloc\n");
        fclose(f);
        return 1;
    }

    for(int i = 0; i < 15 && file_size; i++){
        int res;
        if(i < 12)
            res = read_block(f,  le32toh(inode.i_block[i]), block_size, 0, file_size, buf);
        else
            res = read_block(f,  le32toh(inode.i_block[i]), block_size, i - 11, file_size, buf);
        file_size -= res;
        if(res < 0) {
            perror("Ошибка чтения inode\n");
            free(buf);
            fclose(f);
            return 1;
        }
    }

    if(fill_with_zeros(file_size, block_size, buf) < 0){
        perror("Ошибка fwrite\n");
        free(buf);
        fclose(f);
        return 1;
    }

    free(buf);
    fclose(f);
    
    return 0;
}