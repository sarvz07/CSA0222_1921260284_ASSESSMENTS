#ifndef TOKEN_GLOBAL_H
#define TOKEN_GLOBAL_H

extern int veg_counter;
extern int nonveg_counter;

int next_veg_token(void);
int next_nonveg_token(void);
void issue_from_other_file(void);

#endif
