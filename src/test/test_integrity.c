/*
 * test_function.c
 */

#include "nanoMPK.h"

void set_val(unsigned long *var)
{
  *var = 0xAAAAAAAA;
}

unsigned long read_val(unsigned long *var)
{
  return *var;
}

void set_val_mpk(unsigned long *var)
{
  NF__start__();

  *var = 0xBBBBBBBB;
  // error:
  NF__end__();
  return;
}

unsigned long read_val_mpk(unsigned long *var)
{
  NF__start__();
  unsigned long ret = 0;
  do
  {
    ret = *var;
  } while (0);

  NF__end__();
  // error:
  return ret;
}

unsigned long *create_secret_var()
{
  NF__start__();
  // allocate secret
  unsigned long *var = (unsigned long *)mpk_malloc(sizeof(unsigned long));
  if (var == NULL)
    errExit("allocation of secret failed\n");
  NF__end__();
  return var;
}

int main(int argc, char **argv)
{
  int Size[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  // init isolation and sh mem
  nanoMPK_init(Size);
  unsigned long *var = create_secret_var();

  fprintf(stderr, "var located at %p\n", var);

  set_var_mpk(var);
  fprintf(stderr, "var: %lx\n", read_var_mpk(var));

  fprintf(stderr, "should not segfault: %x\n", __rdpkru());
  fprintf(stderr, "var: %lx\n", read_var(var));

  fprintf(stderr, "should segfault:\n");
  set_var(var);
  nanoMPK_fini(Size);
  return 0;
}
