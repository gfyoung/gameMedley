target = open('englishWordsList.txt', 'r')
words = target.readlines()
target.close()

target = open('englishWordsList.js', 'w')
target.write('var englishWords = [\n')

for word in words:
	target.write('\t"{word}",\n'.format(word=word.strip('\n')))

target.write('];')
target.close()
