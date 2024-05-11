#!/bin/bash



fisier="$1"



if [ ! -f "$fisier" ]; then
    
    echo "Fișierul '$fisier' nu există sau nu este accesibil."
    
    exit 1
    
fi



chmod 777 "$1"

line_count=$(wc -l < "$fisier")

word_count=$(wc -w < "$fisier")

char_count=$(wc -c < "$fisier")



if [ "$line_count" -lt 3 ] && [ "$word_count" -gt 100 ] && [ "$char_count" -gt 200 ]; then
    
    echo " '$fisier' "
    
    
    
    exit 1
    
    
    
else
    
    
    
    if grep -q -e "corrupted" -e "dangerous" -e "risk" -e "attack" -e "malware" -e "malicious" "$fisier"; then
        
        echo " '$fisier' "
        
        
        
        exit 1
        
        
        
    else
        
        
        
        if LC_ALL=C grep -q '[^[:print:]]' "$fisier"; then
            
            echo " '$fisier' "
            
            
            
            exit 1
            
        fi
        
        
        
        
        
        
        
    fi
    
fi

echo "SAFE"





exit 0

