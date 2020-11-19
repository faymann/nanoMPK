/*
 * test_function.c
 */

#include "nanoMPK.h"

int set_var(unsigned long *var, unsigned long val)
{
  *var = val;
  return 0;
}

unsigned long read_var(unsigned long *var)
{
  return *var;
}

NANO_MPKED_FUNC2(int, set_var_mpk, set_var, unsigned long *, unsigned long)

NANO_MPKED_FUNC1(unsigned long, read_var_mpk, read_var, unsigned long *)

unsigned long *create_var()
{
  unsigned long *var = (unsigned long *)mpk_malloc(sizeof(unsigned long));
  if (var == NULL)
    errExit("allocation of secret failed\n");
  return var;
}

NANO_MPKED_FUNC0(unsigned long *, create_secret_var, create_var)

int main(int argc, char **argv)
{
  int Size[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  // init isolation and sh mem
  nanoMPK_init(Size);
  unsigned long *var = create_secret_var();

  fprintf(stderr, "var located at %p\n", var);

  set_var_mpk(var, 0xaaaaaaaa);
  fprintf(stderr, "var: %lx\n", read_var_mpk(var));

  fprintf(stderr, "should not segfault: %x\n", __rdpkru());
  fprintf(stderr, "var: %lx\n", read_var(var));

  fprintf(stderr, "should segfault:\n");
  set_var(var, 0xcccccccc);
  nanoMPK_fini(Size);
  return 0;
}
