#ifndef __INC_CMOS_H
#define __INC_CMOS_H

void cmos_set(unsigned addr, uint8_t val);
void cmos_update(uint8_t IC32, uint8_t sdbval);
void cmos_writeaddr(uint8_t val);
void cmos_write_addr_integra(uint8_t val);
void cmos_write_data_integra(uint8_t val);
uint8_t cmos_read_data_integra(void);
uint8_t cmos_read(void);
void cmos_reset(void);
void cmos_load(const MODEL *m);
void cmos_save(const MODEL *m);

#endif
