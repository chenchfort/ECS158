nmfgpu <- function(a,k)
{
  system("/usr/local/cuda/bin/nvcc -g -Xcompiler -fPIC -c nmfgpur.cu -arch=sm_13 -I/usr/include/R")
  system("R CMD SHLIB nmfgpur.o -lcudart -L/usr/local/cuda/lib64")
  dyn.load("nmfgpur.so")
  .Call("gpu",a, as.integer(k))
}