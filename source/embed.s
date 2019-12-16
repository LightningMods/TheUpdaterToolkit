.section .rodata

    # Kernel Payload for 5.05
    .global kernel_payload_505
    .type   kernel_payload_505, @object
    .align  4
kernel_payload_505:
    .incbin "./kernel/kernel_payload/kernel_payload_505.elf"
kernel_payload_505_end:
    .global kernel_payload_505_size
    .type   kernel_payload_505_size, @object
    .align  4
kernel_payload_505_size:
    .int    kernel_payload_505_end - kernel_payload_505


    # Kernel Payload for 4.74
    .global kernel_payload_474
    .type   kernel_payload_474, @object
    .align  4
kernel_payload_474:
    .incbin "./kernel/kernel_payload/kernel_payload_474.elf"
kernel_payload_474_end:
    .global kernel_payload_474_size
    .type   kernel_payload_474_size, @object
    .align  4
kernel_payload_474_size:
    .int    kernel_payload_474_end - kernel_payload_474

    # OrbisSWU Patch
    .global orbisswu_patch
    .type   orbisswu_patch, @object
    .align  4
orbisswu_patch:
    .incbin "./orbisswu_patch.self"
orbisswu_patch_end:
    .global orbisswu_patch_size
    .type   orbisswu_patch_size, @object
    .align  4
orbisswu_patch_size:
    .int    orbisswu_patch_end - orbisswu_patch
   