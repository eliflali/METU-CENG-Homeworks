from django.shortcuts import redirect


def login_required(view_func):
    def _wrapped_view_func(request, *args, **kwargs):
        # Check if 'token' is in the session
        if 'token' not in request.session:
            # Redirect to the login page if the token is not found
            return redirect('login')
        return view_func(request, *args, **kwargs)

    return _wrapped_view_func
