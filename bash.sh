fisier="$3"
echo "Numele fis este: $fisier"

if [ ! -f "$fisier" ]; then
    echo "Fisierul '$fisier' nu există sau nu este accesibil."
    exit 1
fi

