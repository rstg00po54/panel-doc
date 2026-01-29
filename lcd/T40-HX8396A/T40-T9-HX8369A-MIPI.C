GP_COMMAD_PA(4);SPI_WriteData(0xB9);SPI_WriteData(0xFF);SPI_WriteData(0x83);SPI_WriteData(0x69);

GP_COMMAD_PA(20);SPI_WriteData(0xB1);SPI_WriteData(0x01);SPI_WriteData(0x00);SPI_WriteData(0x34);SPI_WriteData(0x0A);SPI_WriteData(0x00);SPI_WriteData(0x11);SPI_WriteData(0x12);SPI_WriteData(0x21);SPI_WriteData(0x29);SPI_WriteData(0x3F);SPI_WriteData(0x3F);SPI_WriteData(0x01);SPI_WriteData(0x0a);SPI_WriteData(0x01);SPI_WriteData(0xE6);SPI_WriteData(0xE6);SPI_WriteData(0xE6);SPI_WriteData(0xE6);SPI_WriteData(0xE6);

GP_COMMAD_PA(16);SPI_WriteData(0xB2);SPI_WriteData(0x00);SPI_WriteData(0x23);SPI_WriteData(0x03);SPI_WriteData(0x03);SPI_WriteData(0x70);SPI_WriteData(0x00);SPI_WriteData(0xFF);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x03);SPI_WriteData(0x03);SPI_WriteData(0x00);SPI_WriteData(0x01);

GP_COMMAD_PA(6);SPI_WriteData(0xB4);SPI_WriteData(0x02);SPI_WriteData(0x18);SPI_WriteData(0x70);SPI_WriteData(0x13);SPI_WriteData(0x05);

GP_COMMAD_PA(3);SPI_WriteData(0xB6);SPI_WriteData(0x4A);SPI_WriteData(0x4A);

GP_COMMAD_PA(27);SPI_WriteData(0xD5);SPI_WriteData(0x00);SPI_WriteData(0x01);SPI_WriteData(0x03);SPI_WriteData(0x25);SPI_WriteData(0x01);SPI_WriteData(0x04);SPI_WriteData(0x28);SPI_WriteData(0x60);SPI_WriteData(0x11);SPI_WriteData(0x13);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x40);SPI_WriteData(0xE6);SPI_WriteData(0x51);SPI_WriteData(0xF7);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x71);SPI_WriteData(0x35);SPI_WriteData(0x60);SPI_WriteData(0x24);SPI_WriteData(0x07);SPI_WriteData(0x0F);SPI_WriteData(0x04);SPI_WriteData(0x04);

GP_COMMAD_PA(35);SPI_WriteData(0xE0);SPI_WriteData(0x00);SPI_WriteData(0x02);SPI_WriteData(0x0B);SPI_WriteData(0x0A);SPI_WriteData(0x09);SPI_WriteData(0x18);SPI_WriteData(0x1D);SPI_WriteData(0x2A);SPI_WriteData(0x08);SPI_WriteData(0x11);SPI_WriteData(0x0D);SPI_WriteData(0x13);SPI_WriteData(0x15);SPI_WriteData(0x14);SPI_WriteData(0x15);SPI_WriteData(0x0F);SPI_WriteData(0x14);SPI_WriteData(0x00);SPI_WriteData(0x02);SPI_WriteData(0x0B);SPI_WriteData(0x0A);SPI_WriteData(0x09);SPI_WriteData(0x18);SPI_WriteData(0x1D);SPI_WriteData(0x2A);SPI_WriteData(0x08);SPI_WriteData(0x11);SPI_WriteData(0x0D);SPI_WriteData(0x13);SPI_WriteData(0x15);SPI_WriteData(0x14);SPI_WriteData(0x15);SPI_WriteData(0x0F);SPI_WriteData(0x14);

GP_COMMAD_PA(2);SPI_WriteData(0xcc);SPI_WriteData(0x00);

GP_COMMAD_PA(2);SPI_WriteData(0x3a);SPI_WriteData(0x77);

GP_COMMAD_PA(14);SPI_WriteData(0xBA);SPI_WriteData(0x00);SPI_WriteData(0xA0);SPI_WriteData(0xC6);SPI_WriteData(0x00);SPI_WriteData(0x0A);SPI_WriteData(0x00);SPI_WriteData(0x10);SPI_WriteData(0x30);SPI_WriteData(0x6F);SPI_WriteData(0x02);SPI_WriteData(0x11);SPI_WriteData(0x18);SPI_WriteData(0x40);

GP_COMMAD_PA(1);SPI_WriteData(0x11);//DisplayOn
Delay_ms(120);

GP_COMMAD_PA(1);SPI_WriteData(0x29);//DisplayOn
Delay_ms(30);
