%{
#include <stdio.h>
#include "y.tab.h"
%}
%%
[1-9][0-9]* {
}
[A-Z][A-Za-z0-9]* {
}
[a-z][a-z0-9_]* {
}
