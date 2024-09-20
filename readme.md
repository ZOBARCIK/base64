#base64 encoder-decoder\

encoder-decoder works\
image encoder wip\

changes:\
fixed the faulty logic of encode64 function handling the 3rd byte wrongly(typo)\
hence fixed the encode_image\
dropped the 4 byte encoding logic for simplicity\
started decode_image\

todo: \
catching nulls \
back propogation \
fix decode_image malloc issue\
 