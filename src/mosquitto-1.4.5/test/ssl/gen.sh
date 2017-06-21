# This file generates the keys and certificates used for testing mosquitto.
# None of the keys are encrypted, so do not just use this script to generate
# files for your own use.

rm -f *.crt *.key *.csr
for a in root signing; do
	rm -rf ${a}CA/
	mkdir -p ${a}CA/newcerts
	touch ${a}CA/index.txt
	echo 01 > ${a}CA/serial
	echo 01 > ${a}CA/crlnumber
done
rm -rf certs

BASESUBJ="/C=GB/ST=Derbyshire/L=Derby/O=Mosquitto Project/OU=Testing"
SBASESUBJ="/C=GB/ST=Nottinghamshire/L=Nottingham/O=Server/OU=Production"
BBASESUBJ="/C=GB/ST=Nottinghamshire/L=Nottingham/O=Server/OU=Bridge"

# The root CA
openssl genrsa -out test-root-ca.key 1024
openssl req -new -x509 -days 3650 -key test-root-ca.key -out test-root-ca.crt -config openssl.cnf -subj "${BASESUBJ}/CN=Root CA/"

# Another root CA that doesn't sign anything
openssl genrsa -out test-bad-root-ca.key 1024
openssl req -new -x509 -days 3650 -key test-bad-root-ca.key -out test-bad-root-ca.crt -config openssl.cnf -subj "${BASESUBJ}/CN=Bad Root CA/"

# This is a root CA that has the exact same details as the real root CA, but is a different key and certificate. Effectively a "fake" CA.
openssl genrsa -out test-fake-root-ca.key 1024
openssl req -new -x509 -days 3650 -key test-fake-root-ca.key -out test-fake-root-ca.crt -config openssl.cnf -subj "${BASESUBJ}/CN=Root CA/"

# An intermediate CA, signed by the root CA, used to sign server/client csrs.
openssl genrsa -out test-signing-ca.key 1024
openssl req -out test-signing-ca.csr -key test-signing-ca.key -new -config openssl.cnf -subj "${BASESUBJ}/CN=Signing CA/"
openssl ca -config openssl.cnf -name CA_root -extensions v3_ca -out test-signing-ca.crt -infiles test-signing-ca.csr

# An alternative intermediate CA, signed by the root CA, not used to sign anything.
openssl genrsa -out test-alt-ca.key 1024
openssl req -out test-alt-ca.csr -key test-alt-ca.key -new -config openssl.cnf -subj "${BASESUBJ}/CN=Alternative Signing CA/"
openssl ca -config openssl.cnf -name CA_root -extensions v3_ca -out test-alt-ca.crt -infiles test-alt-ca.csr

# Valid server key and certificate.
openssl genrsa -out server.key 1024
openssl req -new -key server.key -out server.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=localhost/"
openssl ca -config openssl.cnf -name CA_signing -out server.crt -infiles server.csr 

# Expired server certificate, based on the above server key.
openssl req -new -days 1 -key server.key -out server-expired.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=localhost/"
openssl ca -config openssl.cnf -name CA_signing -days 1 -startdate 120820000000Z -enddate 120821000000Z -out server-expired.crt -infiles server-expired.csr 

# Valid client key and certificate.
openssl genrsa -out client.key 1024
openssl req -new -key client.key -out client.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=test client/"
openssl ca -config openssl.cnf -name CA_signing -out client.crt -infiles client.csr 

# Expired client certificate, based on the above client key.
openssl req -new -days 1 -key client.key -out client-expired.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=test client expired/"
openssl ca -config openssl.cnf -name CA_signing -days 1 -startdate 120820000000Z -enddate 120821000000Z -out client-expired.crt -infiles client-expired.csr 

# Revoked client certificate, based on a new client key.
openssl genrsa -out client-revoked.key 1024
openssl req -new -days 1 -key client-revoked.key -out client-revoked.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=test client revoked/"
openssl ca -config openssl.cnf -name CA_signing -out client-revoked.crt -infiles client-revoked.csr 
openssl ca -config openssl.cnf -name CA_signing -revoke client-revoked.crt
openssl ca -config openssl.cnf -name CA_signing -gencrl -out crl.pem

# Valid client key and certificate, encrypted (use "password" as password)
openssl genrsa -des3 -out client-encrypted.key 1024
openssl req -new -key client-encrypted.key -out client-encrypted.csr -config openssl.cnf -subj "${SBASESUBJ}/CN=test client encrypted/"
openssl ca -config openssl.cnf -name CA_signing -out client-encrypted.crt -infiles client-encrypted.csr

cat test-signing-ca.crt test-root-ca.crt > all-ca.crt
#mkdir certs
#cp test-signing-ca.crt certs/test-signing-ca.pem
#cp test-root-ca.crt certs/test-root.ca.pem
c_rehash certs
