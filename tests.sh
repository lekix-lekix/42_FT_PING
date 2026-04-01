#!/bin/bash

# =============================================================================
# Test suite de parsing pour ft_ping
# Usage : ./test_ft_ping_parsing.sh [chemin_vers_ft_ping]
# =============================================================================

PING=${1:-"./ft_ping"}
PASS=0
FAIL=0
SKIP=0

RED='\033[0;31m'
GRN='\033[0;32m'
YLW='\033[0;33m'
BLU='\033[0;34m'
DIM='\033[2m'
RST='\033[0m'

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

_run() {
    # Lance la commande avec timeout 1s, capture stdout+stderr et exit code
    output=$(timeout 1s $PING "$@" 2>&1)
    echo $?
}

expect_success() {
    local desc="$1"; shift
    local code
    code=$(_run "$@")
    if [ "$code" -eq 0 ] || [ "$code" -eq 124 ]; then
        # 124 = timeout (ping tourne => parsing OK)
        echo -e "  ${GRN}PASS${RST}  $desc"
        echo -e "        ${DIM}$PING $*${RST}"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}FAIL${RST}  $desc"
        echo -e "        ${DIM}$PING $*${RST}"
        echo -e "        ${DIM}exit=$code | $(timeout 1s $PING "$@" 2>&1 | head -1)${RST}"
        FAIL=$((FAIL + 1))
    fi
}

expect_failure() {
    local desc="$1"; shift
    local code
    code=$(_run "$@")
    if [ "$code" -ne 0 ] && [ "$code" -ne 124 ]; then
        echo -e "  ${GRN}PASS${RST}  $desc"
        echo -e "        ${DIM}$PING $*${RST}"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}FAIL${RST}  $desc  ${YLW}(aurait dû échouer)${RST}"
        echo -e "        ${DIM}$PING $*${RST}"
        FAIL=$((FAIL + 1))
    fi
}

section() {
    echo ""
    echo -e "${BLU}━━━  $1  ━━━${RST}"
}

# -----------------------------------------------------------------------------
# Vérification préalable
# -----------------------------------------------------------------------------

if [ ! -x "$PING" ]; then
    echo -e "${RED}Erreur :${RST} '$PING' introuvable ou non exécutable."
    echo "Usage : $0 [chemin_vers_ft_ping]"
    exit 1
fi

echo ""
echo -e "${BLU}ft_ping — tests de parsing${RST}   (binaire : $PING)"

# =============================================================================
# 1. CAS NOMINAUX — FLAGS SANS VALEUR
# =============================================================================

section "1. Flags booléens seuls"

expect_success "flag court   -v"                1.1.1.1 -v
expect_success "flag long    --verbose"         1.1.1.1 --verbose
expect_success "flag court   -v avant la cible" -v 1.1.1.1
expect_success "flags courts combinés  -vc"     1.1.1.1 -vc

# =============================================================================
# 2. OPTIONS AVEC VALEUR — FORMES ATTENDUES
# Rappel : -p 2  |  --pattern 2  |  --pattern=2  |  p2
# =============================================================================

section "2. Option -c / --count"

expect_success "-c valeur séparée"              1.1.1.1 -c 3
expect_success "-c valeur collée"               1.1.1.1 -c3
expect_success "--count valeur séparée"         1.1.1.1 --count 3
expect_success "--count=valeur"                 1.1.1.1 --count=3

section "3. Option -i / --interval"

expect_success "-i valeur séparée"              1.1.1.1 -i 2
expect_success "-i valeur collée"               1.1.1.1 -i2
expect_success "--interval valeur séparée"      1.1.1.1 --interval 2
expect_success "--interval=valeur"              1.1.1.1 --interval=2

section "4. Option -p / --pattern"

expect_success "-p valeur séparée"              1.1.1.1 -p 0xAB
expect_success "-p valeur collée"               1.1.1.1 -p0xAB
expect_success "--pattern valeur séparée"       1.1.1.1 --pattern 0xAB
expect_success "--pattern=valeur"               1.1.1.1 --pattern=0xAB

section "5. Option -w / --timeout"

expect_success "-w valeur séparée"              1.1.1.1 -w 5
expect_success "-w valeur collée"               1.1.1.1 -w5
expect_success "--timeout valeur séparée"       1.1.1.1 --timeout 5
expect_success "--timeout=valeur"               1.1.1.1 --timeout=5

section "6. Option --ttl (long uniquement)"

expect_success "--ttl valeur séparée"           1.1.1.1 --ttl 64
expect_success "--ttl=valeur"                   1.1.1.1 --ttl=64

# =============================================================================
# 3. COMBINAISONS D'OPTIONS
# =============================================================================

section "7. Combinaisons multiples"

expect_success "flags courts groupés  -vc 5"           1.1.1.1 -vc 5
expect_success "-v -i -c toutes formes courtes"        1.1.1.1 -v -i 1 -c 4
expect_success "-v --count --ttl formes mixtes"        1.1.1.1 -v --count=2 --ttl 128
expect_success "options avant et après la cible"       -v -c 3 1.1.1.1 -i 1
expect_success "toutes les options ensemble"           1.1.1.1 -v -i 1 -c 4 -p 0xFF -w 10 --ttl 64
expect_success "formes longues = partout"              1.1.1.1 --verbose --interval=1 --count=4 --pattern=0xFF --timeout=10 --ttl=64

# =============================================================================
# 4. ORDRE DE LA CIBLE
# =============================================================================

section "8. Position de la cible"

expect_success "cible en premier"               1.1.1.1 -v -c 3
expect_success "cible en dernier"               -v -c 3 1.1.1.1
expect_success "cible au milieu"                -v 1.1.1.1 -c 3
expect_success "cible = hostname"               localhost -v
expect_success "cible = IPv6"                   ::1 -v

# =============================================================================
# 5. VALEURS LIMITES VALIDES
# =============================================================================

section "9. Valeurs limites — acceptées"

expect_success "--count=1   (minimum usuel)"    1.1.1.1 --count=1
expect_success "--count=65535"                  1.1.1.1 --count=65535
expect_success "--interval=0"                   1.1.1.1 --interval=0
expect_success "--ttl=1"                        1.1.1.1 --ttl=1
expect_success "--ttl=255"                      1.1.1.1 --ttl=255
expect_success "--pattern=0"                    1.1.1.1 --pattern=0
expect_success "--pattern=255"                  1.1.1.1 --pattern=255
expect_success "-w 0"                           1.1.1.1 -w 0

# =============================================================================
# 6. CAS D'ERREUR ATTENDUS
# =============================================================================

section "10. Erreurs — option inconnue"

expect_failure "flag court inconnu  -z"         1.1.1.1 -z
expect_failure "flag long inconnu   --foo"      1.1.1.1 --foo
expect_failure "option long inconnu --bar=3"    1.1.1.1 --bar=3

section "11. Erreurs — valeur manquante"

expect_failure "-c sans valeur"                 1.1.1.1 -c
expect_failure "-i sans valeur"                 1.1.1.1 -i
expect_failure "-p sans valeur"                 1.1.1.1 -p
expect_failure "-w sans valeur"                 1.1.1.1 -w
expect_failure "--count sans valeur"            1.1.1.1 --count
expect_failure "--interval sans valeur"         1.1.1.1 --interval
expect_failure "--pattern sans valeur"          1.1.1.1 --pattern
expect_failure "--timeout sans valeur"          1.1.1.1 --timeout
expect_failure "--ttl sans valeur"              1.1.1.1 --ttl

section "12. Erreurs — valeurs invalides"

expect_failure "--count valeur négative"        1.1.1.1 --count=-1
expect_failure "--count valeur non numérique"   1.1.1.1 --count=abc
expect_failure "--ttl=0  (hors plage)"          1.1.1.1 --ttl=0
expect_failure "--ttl=256 (hors plage)"         1.1.1.1 --ttl=256
expect_failure "--interval valeur négative"     1.1.1.1 --interval=-1
expect_failure "--pattern > 255"                1.1.1.1 --pattern=256
expect_failure "--timeout négatif"              1.1.1.1 --timeout=-1

section "13. Erreurs — cible absente"

expect_failure "aucun argument"
expect_failure "uniquement des options sans cible"   -v -c 3

section "14. Erreurs — cible invalide"

expect_failure "cible vide (chaîne vide)"       ""
expect_failure "double cible"                   1.1.1.1 8.8.8.8

# =============================================================================
# 7. EDGE CASES / AMBIGUÏTÉS
# =============================================================================

section "15. Edge cases"

expect_success "-- stoppe le parsing d'options"         1.1.1.1 -v -- -c
expect_failure "option sans tiret en milieu de flags"   1.1.1.1 v
expect_success "-v répétée (idempotent ou ignorée)"     1.1.1.1 -v -v
expect_success "--count=3 puis -c 5 (dernier gagne)"   1.1.1.1 --count=3 -c 5
expect_success "valeur numérique pour pattern sans 0x"  1.1.1.1 -p 42
expect_success "valeur hexa pour pattern avec 0x"       1.1.1.1 -p 0x2A
expect_success "--verbose flag après option à valeur"   1.1.1.1 -c 3 --verbose

# =============================================================================
# BILAN
# =============================================================================

TOTAL=$((PASS + FAIL))
echo ""
echo -e "${BLU}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RST}"
printf "  Résultat : "
if [ $FAIL -eq 0 ]; then
    echo -e "${GRN}$PASS / $TOTAL passés${RST}"
else
    echo -e "${GRN}$PASS${RST} passés  ${RED}$FAIL échoués${RST}  (total $TOTAL)"
fi
echo ""

[ $FAIL -eq 0 ] && exit 0 || exit 1
