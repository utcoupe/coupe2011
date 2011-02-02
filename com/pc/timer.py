# -*- coding: utf-8 -*-


import threading


class MyTimer:
    def __init__(self, tempo, target, args= [], kwargs={}):
        self._target = target
        self._args = args
        self._kwargs = kwargs
        self._tempo = tempo

    def _run(self):
        self._timer = threading.Timer(self._tempo, self._run)
        self._timer.start()
        self._target(*self._args, **self._kwargs)
        
    def start(self):
        self._timer = threading.Timer(self._tempo, self._run)
        self._timer.start()

    def stop(self):
        self._timer.cancel()
	


class InteruptableThreadedLoop(threading.Thread):
	""" Thread qui peut être interompue
	le timeout et l'arret marchent moyennement, la timer ne s'arrete pas tant que la fonction n'est pas finie
	"""
	def __init__(self, group = None, name=None):
		threading.Thread.__init__(self, group, None, name)
		self._stopevent = threading.Event()
		self.result = None
	
	def start(self, target, args=(), kwargs={}, timeout=None, pause=0.0001):
		"""
		@param
			target: la fonction à boucler
			args & kwargs: les args sous forme de liste ou de key/value
			pause: le temps entre chaque appelle de la fonction
		"""
		pause = max(0.0001,pause)
		self._target = target
		self._args = args
		self._kwargs = kwargs
		self._pause = pause
		self._timeout = timeout
		threading.Thread.start(self)
	
	def run(self):
		if self._timeout:
			threading.Timer(self._timeout, self.stop).start()
		while not self._stopevent.isSet():
			self._stopevent.wait(self._pause)
			self.result = self._target(*self._args, **self._kwargs)
		print 'le thread :', self.name, 'est fini'
	
	def stop(self):
		self._stopevent.set()

def timeoutLoop(timeout_duration, target, args=(), kwargs={}, pause=0):
	it = InteruptableThreadedLoop()
	it.start(target, args, kwargs, pause)
	it.join(timeout_duration)
	it.stop()


def timeout(timeout_duration, func, args=(), kwargs={}, name=None, default=None):
    """This function will spawn a thread and run the given function
    using the args, kwargs and return the given default value if the
    timeout_duration is exceeded.
    """
    class InterruptableThread(threading.Thread):
        def __init__(self):
            threading.Thread.__init__(self, name=name)
            print 'start timeout :', name
            self.result = default
        def run(self):
            self.result = func(*args, **kwargs)
    it = InterruptableThread()
    it.start()
    it.join(timeout_duration)
    return it.result
