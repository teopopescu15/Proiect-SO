#!/bin/bash

fisier="$1"
echo "Numele fișierului este: $fisier"

if [ ! -f "$fisier" ]; then
    echo "Fișierul '$fisier' nu există sau nu este accesibil."
    exit 1
fi
line_count=$(wc -l < "$fisier") ;
word_count=$(wc -w < "$fisier");
char_count=$(wc -c < "$fisier");

if [ "$line_count" -lt 3 ] && [ "$word_count" -gt 100 ] && [ "$char_count" -gt 200 ]; then
    echo " '$fisier' "
    exit -1
    
else
    
    if grep -q -e "corrupted" -e "dangerous" -e "risk" -e "attack" -e "malware" -e "malicious" "$fisier"; then
        echo " '$fisier' "
        exit 1
    else
        
        
        # ia fiec caracter
        while IFS= read -r -n1 char; do
            
            # Check if the character is printable
            if (($(printf "%d' "'$char") < 32 || $(printf "%d' "'$char") > 126)); then
                echo " '$fisier' "
                exit 1
            fi
            
        done
    fi
fi
echo "SAFE"



exit 0