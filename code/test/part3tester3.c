
"rt3Tester2.c
 *
  * A test program that first reverse a sorted array, and then
   * reverse back. This tester can verify the functionality of 
    * the backing store and good locality.
     *
      *  If running successfully, the exit return value would be 0.
       */
       
       
       #include "syscall.h"
       
        int A[1000];
        
         int
          main()
           {
             int i, j, tmp;
             
               for (i = 0; i < 1000; i++) {
                  A[i] = 1000 - i;
                    }
                    
                        /* then sort! */
                          for (i = 0; i < 1000; i++) {
                             for (j = 0; j < i; j++) {
                                 if (A[i] < A[j]) { 
                                      tmp = A[j];
                                           A[j] = A[i];
                                                A[i] = tmp;
                                                    }
                                                       }
                                                         }
                                                         
                                                           for (i = 0; i < 1000; i++) {
                                                              A[i] = 1000 - i;
                                                                }
                                                                
                                                                    /* then sort! */
                                                                      for (i = 0; i < 1000; i++) {
                                                                         for (j = 0; j < i; j++) {
                                                                             if (A[i] < A[j]) { 
                                                                                  tmp = A[j];
                                                                                       A[j] = A[i];
                                                                                            A[i] = tmp;
                                                                                                }
                                                                                                   }
                                                                                                     }
                                                                                                     
                                                                                                         Exit(A[0]);    /* and then we're done -- should be 0! */
                                                                                                          }
                                                                                                          ============================================================================
" Netrw Directory Listing                                        (netrw v134)
"   /home/linux/ieng6/oce/1l/sil024/test
"   Sorted by      name
"   Sort sequence: [\/]$,\.h$,\.c$,\.cpp$,*,\.o$,\.obj$,\.info$,\.swp$,\.bak$,\~$
"   Quick Help: <F1>:help  -:go up dir  D:delete  R:rename  s:sort-by  x:exec
" ============================================================================
../
baseboggleplayer.h
boggleUtil.h
boggleboard.h
boggleplayer.h
mainwindow.h
ui_mainwindow.h
boggleUtil.cpp
boggleboard.cpp
boggleplayer.cpp
bogtest.cpp
main.cpp
mainwindow.cpp
moc_mainwindow.cpp
Makefile
P4
P4.pro
boggleref
boglex.txt
mainwindow.ui
.swp
