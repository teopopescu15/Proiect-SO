fisier="$3"
echo "Numele fis este: $fisier"

if [ ! -f "$fisier" ]; then
    echo "Fisierul '$fisier' nu există sau nu este accesibil."
    exit 1
fi
#testez cu grep,oare fac bine?
#contine dangerous?
if grep -q "dangerous" "$fisier"; then  
    echo "Fisierul '$fisier' contine cuvantul 'dangerous'."
    # mv %fisier -t dir_izolat
fi

#  fișierul conține cuvântul "corrupted"
if grep -q "corrupted" "$fisier"; then
    echo "Fisierul '$fisier' contine cuvantul 'corrupted'."
   # mv %fisier -t dir_izolat
fi

