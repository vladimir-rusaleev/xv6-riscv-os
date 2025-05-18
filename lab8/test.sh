#!/bin/bash
set -euo pipefail

BIN="./inode"

echo "Очистка предыдущего состояния"
sudo umount -f mnt 2>/dev/null || true
rm -rf mnt ext2.img extracted*.bin extracted*.txt loop_*.bin

echo "Создание файла-образа ext2"
truncate -s 100M ext2.img

echo "Форматирование образа как ext2"
mkfs.ext2 -q ext2.img

echo "Монтирование файловой системы"
mkdir -p mnt
sudo mount -o loop ext2.img mnt
sudo chown "$(id -u):$(id -g)" mnt

echo "Создание тестовых файлов"
echo "Hello ext2 FS test" > mnt/file1.txt
dd if=/dev/urandom of=mnt/file2.bin bs=1M count=4 status=none
dd if=/dev/zero of=mnt/file3.sparse bs=1M seek=90 count=1 status=none
dd if=/dev/urandom of=mnt/file4_large.bin bs=1M count=70 status=none

echo "Создание тестовых каталогов"
mkdir mnt/test_dir
echo "test" > mnt/test_dir/file_in_dir.txt

sync

echo "Определение номеров inode"
inode1=$(ls -i mnt/file1.txt | awk '{print $1}')
inode2=$(ls -i mnt/file2.bin | awk '{print $1}')
inode3=$(ls -i mnt/file3.sparse | awk '{print $1}')
inode4=$(ls -i mnt/file4_large.bin | awk '{print $1}')

inode_dir=$(ls -id mnt/test_dir | awk '{print $1}')
inode_file_in_dir=$(ls -i mnt/test_dir/file_in_dir.txt | awk '{print $1}')

echo "Вычисление оригинальных SHA512-хэшей"
sha1_orig=$(sha512sum mnt/file1.txt | awk '{print $1}')
sha2_orig=$(sha512sum mnt/file2.bin | awk '{print $1}')
sha3_orig=$(sha512sum mnt/file3.sparse | awk '{print $1}')
sha4_orig=$(sha512sum mnt/file4_large.bin | awk '{print $1}')

echo "Размонтирование файловой системы"
sudo umount mnt

echo "Извлечение данных из образа по inode"
$BIN ext2.img "$inode1" > extracted1.txt
$BIN ext2.img "$inode2" > extracted2.bin
$BIN ext2.img "$inode3" > extracted3.bin
$BIN ext2.img "$inode4" > extracted4.bin

echo "Извлечение каталога"
$BIN ext2.img "$inode_dir" > extracted_dir.bin
if ! grep -q "file_in_dir" extracted_dir.bin; then
    echo "Ошибка: не удалось прочитать каталог"
    exit 1
fi

echo "Подключение loop-устройства"
LOOP=$(sudo losetup --find --show ext2.img)
trap 'sudo losetup -d "$LOOP"' EXIT

echo "Извлечение данных из loop-устройства по inode"
sudo $BIN "$LOOP" "$inode1" > loop_1.txt
sudo $BIN "$LOOP" "$inode2" > loop_2.bin
sudo $BIN "$LOOP" "$inode3" > loop_3.bin
sudo $BIN "$LOOP" "$inode4" > loop_4.bin

echo "Вычисление SHA512-хэшей извлечённых файлов"

sha1_ext=$(sha512sum extracted1.txt | awk '{print $1}')
sha2_ext=$(sha512sum extracted2.bin | awk '{print $1}')
sha3_ext=$(sha512sum extracted3.bin | awk '{print $1}')
sha4_ext=$(sha512sum extracted4.bin | awk '{print $1}')

sha1_loop=$(sha512sum loop_1.txt | awk '{print $1}')
sha2_loop=$(sha512sum loop_2.bin | awk '{print $1}')
sha3_loop=$(sha512sum loop_3.bin | awk '{print $1}')
sha4_loop=$(sha512sum loop_4.bin | awk '{print $1}')

echo -e "\nРезультаты проверки из файла-образа:"
for i in 1 2 3 4; do
  orig_var=sha${i}_orig
  ext_var=sha${i}_ext
  echo -e "\nФайл $i:"
  echo "  Оригинал:    ${!orig_var}"
  echo "  Извлечено:   ${!ext_var}"
  if [[ "${!orig_var}" == "${!ext_var}" ]]; then
    echo -e "  Ок"
  else
    echo -e "  Не ок"
  fi
done

echo -e "\nРезультаты проверки из loop-устройства:"
for i in 1 2 3 4; do
  orig_var=sha${i}_orig
  loop_var=sha${i}_loop
  echo -e "\nФайл $i:"
  echo "  Оригинал:    ${!orig_var}"
  echo "  Извлечено с loop: ${!loop_var}"
  if [[ "${!orig_var}" == "${!loop_var}" ]]; then
    echo -e "  Ок"
  else
    echo -e "  Не ок"
  fi
done

echo -e "\nОчистка временных файлов"
rm -rf mnt ext2.img extracted*.bin extracted*.txt loop_*.bin loop_*.txt

echo -e "\nВсе тесты завершены"
