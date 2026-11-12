--- src/libthr/pthread.c.orig	2026-11-10 15:23:08.000000000 -0800
+++ src/libthr/pthread.c	2026-11-11 13:58:29.351403000 -0800
@@ -222,6 +222,7 @@ static void init_mutex_if_necessary(linux_pthread_mute
       pthread_mutexattr_settype(&attr, type);
 
       pthread_mutex_init(NATIVE_MUTEX_T(mutex), &attr);
+      pthread_mutexattr_destroy(&attr);
     }
 
     int unlock_err = pthread_mutex_unlock(&mutex_initialization_mutex);
@@ -248,7 +249,9 @@ static int shim_pthread_mutex_consistent_impl(linux_pt
 }
 
 static int shim_pthread_mutex_consistent_impl(linux_pthread_mutex_t* mutex) {
-  assert(mutex->_wrapped_mutex != 0);
+  if (mutex->_wrapped_mutex == NULL) {
+    return EINVAL;
+  }
   return pthread_mutex_consistent(NATIVE_MUTEX_T(mutex));
 }
 
@@ -269,12 +272,20 @@ static int shim_pthread_mutex_destroy_impl(linux_pthre
 SHIM_WRAP(pthread_mutex_trylock);
 
 static int shim_pthread_mutex_destroy_impl(linux_pthread_mutex_t* mutex) {
-  assert(mutex->_wrapped_mutex != 0);
-  return pthread_mutex_destroy(NATIVE_MUTEX_T(mutex));
+  if (mutex->_wrapped_mutex == NULL) {
+    return 0;
+  }
+  int err = pthread_mutex_destroy(NATIVE_MUTEX_T(mutex));
+  if (err == 0) {
+    mutex->_wrapped_mutex = NULL;
+  }
+  return err;
 }
 
 static int shim_pthread_mutex_unlock_impl(linux_pthread_mutex_t* mutex) {
-  assert(mutex->_wrapped_mutex != 0);
+  if (mutex->_wrapped_mutex == NULL) {
+    return EINVAL;
+  }
   return pthread_mutex_unlock(NATIVE_MUTEX_T(mutex));
 }
 
