cp ./Debug/x86_64_prj.elf  root
grub-mkrescue -o multiboot.iso root

qemu-system-x86_64 -cdrom multiboot.iso -cpu host -enable-kvm -boot d  -S -s
